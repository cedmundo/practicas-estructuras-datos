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

#include "postfixy.h"
#include "evaly.h"
#include "stacky.h"

#include <math.h>
#include <stdio.h>

/// AB+C*
//  ---> @ = AB+
//    @C*
//    ---> @ = RC*
//      @

/// ABC+*
//   ---> @ = BC+
//    A@*
//    ---> @ = A@*
//      @

//  A*B/C+D
//  ABCD*/+

double evaluate (Stack * expression){
	StackType operation;
	StackType right;
	StackType left;

	Stack tmpval;
	init_stack(&tmpval);

	printf("\n");	
	print_stack(expression);
	printf("\n");

	StackType cur_sys_var = *(current_system_variable());

	if (expression->top == 0){
		//printf("Finalizado, obteniendo resultado en %c\n", cur_sys_var-1);
		return get_variable_value(cur_sys_var-1);
	}else{
		*(current_system_variable()) = cur_sys_var + 1;
	}

	left = pop(expression);
	right = pop(expression);
	operation = pop(expression);

	while (!is_empty(expression)){
		//printf("Segmento: %c %c %c\n", left, operation, right);
		if ( is_supported_operator(operation) && is_variable_ext(left) && is_variable_ext(right) )
			break;

		push(&tmpval, left);
		left = right;
		right = operation;
		operation = pop(expression);
	}
	//printf("Encontrado: %c %c %c\n", left, operation, right);

	double a = get_variable_value(right);
	double b = get_variable_value(left);
	double r = 0;

	switch(operation){
	case '+': r = a + b; break;
	case '-': r = a - b; break;
	case '*': r = a * b; break;
	
	case '/': 
		if (b == 0){
			//printf("\nDivisión entre cero: %d\n", expression->top);
			return 0;
		}
		
		r = a / b; 
		break;
		
	case '^': r = pow(a, b); break;
	}

	printf("Evaluando: %c:%lf %c %c:%lf = %c:%lf\n", left, a, operation, right, b, cur_sys_var,r);
	set_variable_value(cur_sys_var, r);
	push(expression, cur_sys_var);
	reverse(&tmpval, expression);

	return evaluate(expression);
}

void reverse(Stack * origin, Stack * result){
	while (!is_empty(origin)){
		push (result, pop(origin));
	}
}

double get_variable_value(StackType variable){

	if ( is_in_program_space(variable) )
		return get_program_variables()[ (int32_t) variable - 'a'];
	else
		return get_user_variables()[ (int32_t) variable - 'A'];
}

void set_variable_value(StackType variable, double value){
	
	if ( is_in_program_space(variable) )
		get_program_variables()[ (int32_t) variable - 'a'] = value;
	else
		get_user_variables()[ (int32_t) variable - 'A'] = value;
}

void fill_used(Stack * s){
	int32_t i;
	char a;

	for(i=0;i<s->top;i++){
		a = s->contents[i];

		if (is_variable(a)){
			get_variable_use()[ (int32_t) a - 'A'] = 1;
		}
	}
}

int8_t is_in_program_space (StackType var){
	return var >= 'a' && var <= 'z';
}

int8_t * get_variable_use(){
	static int8_t used [] = {
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0
	};

	return used;
}

double * get_user_variables(){
	static double user_variables [] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.,  };

	return user_variables;
}

double * get_program_variables(){
	static double program_variables [] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0 };

	return program_variables;
}

StackType * current_system_variable(){
	static StackType cur_sys_var = 'a';
	return &cur_sys_var;
}