#include "recode57.h"
#include <stdio.h>

enum encoding bom_to_enc(uint8_t *bom){
    
    if(bom[0] == 0xFE && bom[1] == 0xFF) // los dos primeros bytes son FE FF ---> UTF-16BE
        return UTF16BE;       
    else if(bom[0] == 0 && bom[1] == 0 && bom[2] == 0xFE && bom[3] == 0xFF) // los cuatro primeros bytes son 00 00 FE FF ---> UTF-32BE               
        return UTF32BE;    
    else if(bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0 && bom[3] == 0) // los cuatro primeros bytes son FF FE 00 00 ---> UTF-32LE
        return UTF32LE;    
    else if(bom[0] == 0xFF && bom[1] == 0xFE) // los dos primeros son FF FE ---> UTF-16LE
        return UTF16LE;    
    else // ---> UTF-8
        return UTF8;
}

size_t write_bom(enum encoding enc, uint8_t* buf){

    if(enc == UTF16BE){
        buf[0] = 0xFE;
        buf[1] = 0xFF;
        return 2;
    }
    else if(enc == UTF32BE){       
        buf[0] = 0x00;          
        buf[1] = 0x00;
        buf[2] = 0xFE;
        buf[3] = 0xFF;
        return 4;    
    }
    else if(enc == UTF32LE){
        buf[0] = 0xFF;          
        buf[1] = 0xFE;
        buf[2] = 0x00;
        buf[3] = 0x00;
        return 4;    
    }
    else if(enc == UTF16LE){
        buf[0] = 0xFF;
        buf[1] = 0xFE;
        return 2; 
    } 
    else{
        return 0;
    } 
}

bool has_codepoint(enum encoding enc, uint8_t *buf, size_t nbytes){

    if(enc == UTF32BE || enc == UTF32LE){
        if(nbytes < 4)
            return false;                               
    }
    else if(enc == UTF16BE){          
        if(nbytes < 2)                                  //  D8 00     high surrogates  
                return false;                           //  DB FF                             
        if(nbytes < 4){                                 //----------------------------    
            if(buf[0] >= 0xD8 && buf[0] <= 0xDB)        //  DC 00     low surrogate         
                return false;                           //  DF FF                          
        }
    }
    else if(enc == UTF16LE){
        if(nbytes < 2)                                 
                return false;
        if(nbytes < 4){                                
            if(buf[1] >= 0xD8 && buf[1] <= 0xDB)
                return false;
        }        
    }else if(enc == UTF8){
        if(nbytes == 0)
            return false;        
    }
    return true;                                   
}

size_t read_codepoint(enum encoding enc, uint8_t *buf, size_t nbytes, codepoint_t *dest){
    
    codepoint_t cp = 0;
    codepoint_t cp1 = 0;
    codepoint_t cp2 = 0;
    codepoint_t cp3 = 0;
    codepoint_t cp4 = 0;
    
    if(has_codepoint(enc, buf, nbytes)){
        if(enc == UTF32BE){                                                   
            for(int i = 0; i < 3; i++){                  
                cp = (cp | buf[i]) << 8;             
            }
            cp = (cp | buf[3]);                                          
            (*dest) = cp;                                 
            return 4;
        }else if(enc == UTF32LE){
            for(int i = 3; i > 0; i--){
                cp = (cp | buf[i]) << 8;
            }
            cp = (cp | buf[0]);
            (*dest) = cp;
            return 4;
        }else if(enc == UTF16BE || enc == UTF16LE){
            if(nbytes >= 2){
            	if(enc == UTF16BE){
                    if(buf[0] < 0xD8 || buf[0] > 0xDB){
                        // No es surrogate.
                	    cp = ((cp | buf[0]) << 8);
                	    cp =  (cp | buf[1]);
                        (*dest) = cp;
                        return 2;                          
                    }else{
                        if(nbytes >= 4){
                            // Caso con surrogates.
                            cp = (cp | (buf[0] & 3)) << 8;
                            cp = (cp | buf[1]) << 2;
                            cp = (cp | (buf[2] & 3)) << 8;
                            cp = cp | buf[3];
                            cp = cp + 0x10000;
                            (*dest) = cp;
                            return 4;
                        }else
                            return 0;
                    }
            	}else if(enc == UTF16LE){
                    if(buf[1] < 0xD8 || (buf[1] > 0xDB)){ 
                        // No es surrogate.
            		    cp = ((cp | buf[1]) << 8);
                	    cp =  (cp | buf[0]); 
                        (*dest) = cp;
                        return 2;
                    }else{
                        if(nbytes >= 4){
                            // Caso con surrogates.
                            cp = (cp | (buf[1] & 3)) << 8;
                            cp = (cp | buf[0]) << 2;
                            cp = (cp | (buf[3] & 3)) << 8;
                            cp = cp | buf[2];                      
                            cp = cp + 0x10000;
                            (*dest) = cp;
                            return 4;                            
                        }else
                            return 0;
                    } 
                }
            }
        }else if(enc == UTF8){
            if(buf[0] < 0x80){  
                (*dest) = buf[0];
                return 1;
            }else if(buf[0] < 0xE0){
                cp1 = buf[0] & 0x1F;
                cp2 = buf[1] & 0x3F;
                cp = ((cp1 << 6) | cp2);
                (*dest) = cp;                
                return 2;
            }else if(buf[0] < 0xF0){
                cp1 = buf[0] & 0x0F;
                cp2 = buf[1] & 0x3F;
                cp3 = buf[2] & 0x3F;
                cp = ((cp1 << 12) | (cp2 << 6) | cp3);
                (*dest) = cp;                
                return 3;                    
            }else if(buf[0] < 0xF8){
                cp1 = buf[0] & 0x7;
                cp2 = buf[1] & 0x3F;
                cp3 = buf[2] & 0x3F;
                cp4 = buf[3] & 0x3F;
                cp = ((cp1 << 18) | (cp2 << 12) | (cp3 << 6) | cp4);
                (*dest) = cp;
                return 4;
            }                                    
        }
    }
    return 0;    
}

size_t write_codepoint(enum encoding enc, codepoint_t codepoint, uint8_t *outbuf){

    codepoint_t aux = 0xFF;

    if(enc == UTF32BE){
    	for(int i = 3; i > 0; i--){
    		aux = (codepoint & 0xFF);
    		outbuf[i] = aux;
        	codepoint = codepoint >> 8; 
    	}
        aux = (codepoint & 0xFF);
        outbuf[0] = aux;      
        return 4;
    }else if(enc == UTF32LE){
    	for(int i = 0; i < 3; i++){
    		aux = (codepoint & 0xFF);
    		outbuf[i] = aux;
        	codepoint = codepoint >> 8;
    	}
        aux = (codepoint & 0xFF);
        outbuf[3] = aux;
        return 4;
    }else if(enc == UTF16BE || enc == UTF16LE){
        if(codepoint <= 0xFFFF){
        	if(enc == UTF16BE){
	            // sin surrogatess.
	            outbuf[1] = (codepoint & 0xFF);
	            outbuf[0] = (codepoint >> 8) & 0xFF;
	            return 2;        		
        	}else if(enc == UTF16LE){
	            outbuf[0] = (codepoint & 0xFF);
	            outbuf[1] = (codepoint >> 8) & 0xFF;
	            return 2;
        	}
        }else{
            // con surrogatess.
            codepoint = codepoint - 0x10000;
            if(enc == UTF16BE){
                outbuf[0] = 0xD8 | (codepoint >> 18);
                outbuf[1] = (codepoint >> 10) & 0xFF;
                outbuf[2] = 0xDC | ((codepoint >> 8) & 3);
                outbuf[3] = codepoint & 0xFF;
                return 4;                                
            }else if(enc == UTF16LE){
                outbuf[0] = (codepoint >> 10) & 0xFF;
                outbuf[1] = 0xD8 | (codepoint >> 18);
                outbuf[2] = codepoint & 0xFF;
                outbuf[3] = 0xDC | ((codepoint >> 8) & 3);
                return 4;
            }
        }
    }else if(enc == UTF8){
        if(codepoint < 0x80){
            outbuf[0] = codepoint;
            return 1;
        }if(codepoint < 0x800){ // 128 a 2047 ---> 2 bytes 
            outbuf[0] = 0xC0 | ((codepoint >> 6) & 0x8F);
            outbuf[1] = 0x80 | (codepoint & 0x3F);
            return 2;
        }if(codepoint < 0x10000){ // 2048 a 65535 ---> 3 bytes
            outbuf[0] = 0xE0 | ((codepoint >> 12) & 0x8F);
            outbuf[1] = 0x80 | ((codepoint >> 6) & 0x3F);
            outbuf[2] = 0x80 |(codepoint & 0x3f);
            return 3;
        }if(codepoint <= 0x110000){ // 65536 a 1114111 ---> 4 bytes
            outbuf[0] = 0xF0 | ((codepoint >> 18) & 0x8F);
            outbuf[1] = 0x80 | ((codepoint >> 12) & 0x3F);
            outbuf[2] = 0x80 | ((codepoint >> 6) & 0x3F);
            outbuf[3] = 0x80 | (codepoint & 0x3F);
            return 4;
        }
    }
    return 0;
}