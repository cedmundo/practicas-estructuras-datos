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
	Stack invtmp;
	
	init_stack(&tmpval);
	init_stack(&invtmp);
	
	if (expression->top == 0){
		return get_variable_value(K_RESULT_VARIABLE);
	}

	left = pop(expression);
	right = pop(expression);
	operation = pop(expression);

	while( is_variable(operation) || operation == '@' ){
		push(&tmpval, left);
		left = right;
		right = operation;
		operation = pop(expression);
	}

	if ( !(is_variable(left) || left == '@') || !(is_variable(right) || right == '@') ){
		printf("\nSintaxis inválida: %d\n", expression->top);
		return 0;
	}

	double a = get_variable_value(left);
	double b = get_variable_value(right);
	double r = 0;

	switch(operation){
	case '+': r = a + b; break;
	case '-': r = a - b; break;
	case '*': r = a * b; break;
	
	case '/': 
		if (b == 0){
			printf("\nDivisión entre cero: %d\n", expression->top);
			return 0;
		}
		
		r = a / b; 
		break;
		
	case '^': r = pow(b, a); break;
	}

	printf("Evaluando: %lf (%c) %c %lf (%c) = %lf\n", a, left, operation, b, right, r);
	print_stack(expression);
	printf("\n");

	set_variable_value(K_RESULT_VARIABLE, r);

	push(expression, K_RESULT_VARIABLE);

	reverse(&tmpval, &invtmp);
	reverse(&invtmp, expression);

	return evaluate(expression);
}

void reverse(Stack * origin, Stack * result){
	while (!is_empty(origin)){
		push (result, pop(origin));
	}
}

double get_variable_value(StackType variable){
	return get_variables()[ (int32_t) variable - '@'];
}

void set_variable_value(StackType variable, double value){
	get_variables()[ (int32_t) variable - '@'] = value;
}

void fill_used(Stack * s){
	int32_t i;
	char a;

	for(i=0;i<s->top;i++){
		a = s->contents[i];

		if (is_variable(a)){
			get_variable_use()[ (int32_t) a - '@'] = 1;
		}
	}
}

int8_t * get_variable_use(){
	static int8_t used [] = {
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0
	};

	return used;
}

double * get_variables(){
	static double variables [] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0 };

	return variables;
}
