#pragma once

typedef void callback_procedure( 
	void* object_reference1, 
	void* object_referencec2);

typedef struct callback_structure
{
	callback_procedure* callback;
	void* argument1;
	void* argument2;
} callback_structure;