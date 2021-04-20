use crate::Codepoint;
use crate::Encoding::{self, *};
use std::cmp::min;

// Esta función es idéntica a la versión en C, excepto que también
// devuelve el tamaño del bom que se encontró. Esto permite a main.rs
// procesar los bytes restantes.
pub fn bom_to_enc(bom: &[u8; 4]) -> (Encoding, usize) {
    
    match bom {
        [0xFE, 0xFF, _, _] => (UTF16BE, 2),
        [0, 0, 0xFE, 0xFF] => (UTF32BE, 4),
        [0xFF, 0xFE, 0, 0] => (UTF32LE, 4),
        [0xFF, 0xFE, _, _] => (UTF16LE, 2),
        _ => (UTF8, 0),
    }
}

pub fn write_bom(enc: Encoding, buf: &mut [u8; 4]) -> usize {

    match enc {
        UTF16BE => {
            buf[0] = 0xfe;
            buf[1] = 0xff;
            2    
        },
        UTF32BE => {
            buf[0] = 0x00;          
            buf[1] = 0x00;
            buf[2] = 0xfe;
            buf[3] = 0xff;
            4      
        },
        UTF32LE => {
            buf[0] = 0xff;          
            buf[1] = 0xfe;
            buf[2] = 0x00;
            buf[3] = 0x00;
            4            
        },
        UTF16LE => {
            buf[0] = 0xff;
            buf[1] = 0xfe;
            2
        },
        _ => 0,
    }
}

fn has_codepoint(enc: Encoding, input: &[u32], nbytes: usize) -> bool {

    if enc == UTF32BE || enc == UTF32LE {
        if nbytes < 4 {
            return false;                               
        }
    } else if enc == UTF16BE {          
        if nbytes < 2 {                                 
                return false;
            }                         
        if nbytes < 4 {
            if input[0] >= 0xd8 && input[0] <= 0xdb {       
                return false;
            }                                           
        }
    } else if enc == UTF16LE {
        if nbytes < 2 {                                
                return false;
            }
        if nbytes < 4 {                                
            if input[1] >= 0xd8 && input[1] <= 0xdb {
                return false;
            }
        }        
    }else if enc == UTF8 {
        if  nbytes == 0 {
            return false;
        }        
    }
    return true;   
}

// Esta función devuelve lo mismo que su equivalente en C (el codepoint, y la
// cantidad de bytes que se usaron para producirlo), preo en una tupla (ya que
// Rust tiene soporte nativo). Además, en lugar de usar el 0 como indicador que
// no había suficientes bytes para producir un codepoint, se usa None, mucho más
// idiomático en Rust.
pub fn read_codepoint(enc: Encoding, input: &[u8]) -> Option<(Codepoint, usize)> {

    let mut bytes = 0; // Cantidad de bytes consumidos al leer el codepoint.
    let n = input.len(); // Cantidad de bytes disponibles.
    let buf = input[..min(n, 4)].iter().map(|&x| x as u32).collect::<Vec<_>>();
    
    let mut cp: u32 = 0;
    let cp1: u32;
    let cp2: u32;
    let cp3;
    let cp4;

    if has_codepoint(enc, &buf, n) {
        if enc == UTF32BE {   
            cp = (cp | buf[0]) << 8;
            cp = (cp | buf[1]) << 8;
            cp = (cp | buf[2]) << 8;
            cp = cp | buf[3];
            bytes = 4;                      
        }else if enc == UTF32LE {
            cp = (cp | buf[3]) << 8;
            cp = (cp | buf[2]) << 8;
            cp = (cp | buf[1]) << 8;
            cp = cp | buf[0];
            bytes = 4;
        }else if enc == UTF16BE || enc == UTF16LE {
            if n >= 2 {
                if enc == UTF16BE {
                    if buf[0] < 0xd8 || buf[0] > 0xdb {
                        // No es surrogate.
                        cp = (cp | buf[0]) << 8;
                        cp =  cp | buf[1];
                        bytes = 2;                          
                    }else{
                        if n >= 4 {
                            // Caso con surrogates.
                            cp = (cp | (buf[0] & 3)) << 8;
                            cp = (cp | buf[1]) << 2;
                            cp = (cp | (buf[2] & 3)) << 8;
                            cp = cp | buf[3];
                            cp = cp + 0x10000;
                            bytes = 4;
                        }else {
                            bytes = 0;
                        }    
                    }
                }else if enc == UTF16LE {
                    if buf[1] < 0xd8 || (buf[1] > 0xdb) { 
                        // No es surrogate.
                        cp = (cp | buf[1]) << 8;
                        cp =  cp | buf[0]; 
                        bytes = 2;
                    }else{
                        if n >= 4 {
                            // Caso con surrogates.
                            cp = (cp | (buf[1] & 3)) << 8;
                            cp = (cp | buf[0]) << 2;
                            cp = (cp | (buf[3] & 3)) << 8;
                            cp = cp | buf[2];                      
                            cp = cp + 0x10000;
                            bytes = 4;                            
                        }else {
                            bytes = 0;
                        }
                    } 
                }
            }
        }else {
            if buf[0] < 0x80 {  
                cp = buf[0];
                bytes = 1;
            }else if buf[0] < 0xe0 { // 2 bytes
                cp1 = buf[0] & 0x1f; 
                cp2 = buf[1] & 0x3f;
                cp = (cp1 << 6) | cp2;                
                bytes = 2;
            }else if buf[0] < 0xf0 { // 3 bytes
                cp1 = buf[0] & 0x0f;
                cp2 = buf[1] & 0x3f;
                cp3 = buf[2] & 0x3f;
                cp = (cp1 << 12) | (cp2 << 6) | cp3;            
                bytes = 3;                    
            }else if buf[0] < 0xf8 { // 4 bytes
                cp1 = buf[0] & 0x7;
                cp2 = buf[1] & 0x3f;
                cp3 = buf[2] & 0x3f;
                cp4 = buf[3] & 0x3f;
                cp = (cp1 << 18) | (cp2 << 12) | (cp3 << 6) | cp4;
                bytes = 4;
            }                        
        }
    }

    if bytes > 0 {
        return Some((cp, bytes));
    } else {
        None
    }
}

pub fn write_codepoint(enc: Encoding, cp: Codepoint, outbuf: &mut [u8; 4]) -> usize {
    
    let mut cp_recibido: u32 = cp; // Se crea ya que el codepoint recibido por parametro no es mutable.
  
    if enc == UTF32BE {
        outbuf[3] = (cp_recibido & 0xff) as u8;
        outbuf[2] = (cp_recibido >> 8 & 0xff) as u8;
        outbuf[1] = (cp_recibido >> 16 & 0xff) as u8;
        outbuf[0] = (cp_recibido >> 24 & 0xff) as u8;
        return 4;
    }else if enc == UTF32LE {
        outbuf[0] = (cp_recibido & 0xff) as u8;
        outbuf[1] = (cp_recibido >> 8 & 0xff) as u8;
        outbuf[2] = (cp_recibido >> 16 & 0xff) as u8;
        outbuf[3] = (cp_recibido >> 24 & 0xff) as u8;
        return 4;
    }else if enc == UTF16BE || enc == UTF16LE {
        if cp_recibido <= 0xffff {
            if enc == UTF16BE {
                // sin surrogatess.
                outbuf[1] = (cp_recibido & 0xff) as u8;
                outbuf[0] = (cp_recibido >> 8 & 0xff) as u8;
                return 2;               
            } else {
                outbuf[0] = (cp_recibido & 0xff) as u8;
                outbuf[1] = (cp_recibido >> 8 & 0xff) as u8;
                return 2;
            }
        }else{
            // con surrogatess.
            cp_recibido = cp_recibido - 0x10000;
            if enc == UTF16BE {
                outbuf[0] = (0xd8 | (cp_recibido >> 18)) as u8;
                outbuf[1] = ((cp_recibido >> 10) & 0xff) as u8;
                outbuf[2] = (0xdc | ((cp_recibido >> 8) & 3)) as u8;
                outbuf[3] = (cp_recibido & 0xff) as u8;
                return 4;                                
            }else {
                outbuf[0] = ((cp_recibido >> 10) & 0xff) as u8;
                outbuf[1] = (0xd8 | (cp_recibido >> 18)) as u8;
                outbuf[2] = (cp_recibido & 0xff) as u8;
                outbuf[3] = (0xdc | ((cp_recibido >> 8) & 3)) as u8;
                return 4;
            }
        }
    }else {
        if cp_recibido <= 0x7F { 
            outbuf[0] = cp_recibido as u8;
            return 1;
        }else if cp_recibido <= 0x7FF { // 128 a 2047 ---> 2 bytes
            outbuf[0] = (0xC0 | ((cp_recibido >> 6) & 0x8F)) as u8;
            outbuf[1] = (0x80 | (cp_recibido & 0x3F)) as u8;
            return 2;
        }else if cp_recibido <= 0xFFFF { // 2048 a 65535 ---> 3 bytes
            outbuf[0] = (0xE0 | ((cp_recibido >> 12) & 0x8F)) as u8;
            outbuf[1] = (0x80 | ((cp_recibido >> 6) & 0x3F)) as u8;
            outbuf[2] = (0x80 | (cp_recibido & 0x3F)) as u8;
            return 3;
        }else { // 65536 a 1114111 ---> 4 bytes
            outbuf[0] = (0xF0 | ((cp_recibido >> 18) & 0x8F)) as u8;
            outbuf[1] = (0x80 | ((cp_recibido >> 12) & 0x3F)) as u8;
            outbuf[2] = (0x80 | ((cp_recibido >> 6) & 0x3F)) as u8;
            outbuf[3] = (0x80 | (cp_recibido & 0x3F)) as u8;
            return 4;
        }
    }
}