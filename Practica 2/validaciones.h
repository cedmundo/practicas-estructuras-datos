// Copyright (c) 2014, Carlos Edmundo Martínez Mendoza 
// All rights reserved. 

// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 

//  * Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//  * Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the distribution. 
//  * Neither the name of  nor the names of its contributors may be used to 
//    endorse or promote products derived from this software without specific 
//    prior written permission. 

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
 
#ifndef __VALIDACIONES_H__
#define __VALIDACIONES_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "stacky.h"
#include "postfixy.h"
#include "sci.h"

static ValidationResult validate_only_numbers (const SecuredBuffer *buffer) {
    const char *raw = buffer->raw_data;
    const int32_t len = buffer->size;

    int64_t i;

    if (len == 0){
        printf("ERROR: Favor de ingresar un número.\n");
        return K_SCI_AGAIN;
    }

    for (i=0;i<len;i++){
        if ( !( (raw[i] >= '0' && raw[i] <= '9') || raw[i] == '.' || raw[i] == '-') ){
            printf("ERROR: Favor de ingresar un número real (positivo o negativo).\n");
            return K_SCI_AGAIN;
        }
    }

    return K_SCI_CONTINUE;
}

static ValidationResult validate_infixed_syntax (const SecuredBuffer *buffer){
    const char *raw = buffer->raw_data;
    const int32_t len = buffer->size;

    char curbyte, pastbyte;
    int8_t is_cur_variable, is_cur_operation, is_cur_modifier, is_past_modifier;
    int32_t parentheses_couting = 0;
    int64_t i;
    
    if (len >= buffer->capacity){
        printf("ERROR: La cadena que usted ingresó sobrepasa el límite de lectura, ingrese una operación más pequeña.\n");
        return K_SCI_AGAIN;
    }

    if (len == 0){
        printf("ERROR: Favor de ingresar una operación.\n");
        return K_SCI_AGAIN;
    }

    if (len < 3){
        printf("ERROR: Cantidad de operandos y/o operadores inferior a la mínima requerida (>2)\n");
        return K_SCI_AGAIN;
    }

    for (i=0;i<len;i++){
        curbyte = toupper(raw[i]);

        if (i > 0)
            pastbyte = toupper(raw[i-1]);
        else
            pastbyte = 0;

        is_cur_variable = is_variable(curbyte);
        is_cur_operation = is_supported_operator(curbyte);
        is_cur_modifier = increases_prioriy(curbyte) || decreases_prioriy(curbyte);
        is_past_modifier = increases_prioriy(pastbyte) || decreases_prioriy(pastbyte);

        parentheses_couting = 
            increases_prioriy(curbyte) ? parentheses_couting + 1 : 
            (decreases_prioriy(curbyte) ? parentheses_couting - 1 : parentheses_couting);

        if ( is_space(curbyte) ){
            continue;
        }

        if ( (increases_prioriy(curbyte) && is_variable(pastbyte)) || 
             (is_variable(curbyte) && decreases_prioriy(pastbyte)) ){
            printf("ERROR: No se permiten las expresiones tipo A(B) o (B)A, puesto que no se soportan las funciones"
                    " trascendentales ni la multiplicación implícita: %ld.\n", i+1);
            return K_SCI_AGAIN;
        }

        if (is_cur_modifier){
            continue;
        }
        
        if (!is_cur_variable && !is_cur_operation && !is_cur_modifier){
            printf("ERROR: En pocisión %ld ('%c'), solamente se admiten variables "
                    "(A,B,C,...,Z) y operadores +,-,/,*,^,(,),[,]\n", i+1, curbyte);
            return K_SCI_AGAIN;
        }

        if (is_cur_variable && pastbyte != 0 && is_variable(pastbyte)){
            printf("ERROR: Error en la pocisión %ld y %ld: No puede haber dos variables juntas.\n", i, i+1);
            return K_SCI_AGAIN;
        }

        if ( !(is_cur_operation && is_past_modifier) ){
            if (is_cur_operation && pastbyte != 0 && is_supported_operator(pastbyte) ){
                printf("ERROR: Error en la pocisión %ld y %ld: No puede haber dos operadores juntos.\n", i, i+1);
                printf("AVISO: No se soporta el operador negativo aún.\n");
                return K_SCI_AGAIN;
            }
        }

        if ( (is_cur_operation && i == 0) || (is_cur_operation && i == len-1) ){
            printf("ERROR: El valor ingresado puede causar una salida erronea.\n");
            return K_SCI_AGAIN;   
        }

    }

    if (parentheses_couting != 0){
        printf("ERROR: Verifique los paréntesis, no ha abierto o cerrado alguno adecuadamente.\n");
        return K_SCI_AGAIN; 
    }

    return K_SCI_CONTINUE;
}

#endif