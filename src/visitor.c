#include "include/visitor.h"
#include <stdio.h>
#include <string.h>
#include "include/scope.h"

static AST_T* builtin_function_print(visitor_T* visitor, AST_T** args, int args_size)
{
    //printf("Makes it here");
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_STRING: printf("%s\n", visited_ast->string_value); break;
            default: printf("%p\n", visited_ast); break;

        }
    }
    return init_ast(AST_NOOP);
}
visitor_T* init_visitor()
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));

    return visitor;
}
AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
    switch (node->type)
    {
        case AST_VARIABLE_DEFINITION: return visitor_visit_variable_definition(visitor, node); break;
        case AST_FUNCTION_DEFINITION: return visitor_visit_function_definition(visitor, node); break;
        case AST_VARIABLE: return visitor_visit_variable(visitor, node); break;
        case AST_FUNCTION_CALL: return visitor_visit_function_call(visitor, node); break;
        case AST_STRING: return visitor_visit_string(visitor, node); break;
        case AST_COMPOUND: return visitor_visit_compound(visitor, node); break;
        case AST_NOOP: return node; break;
    }

    printf("Uncaught statement of type '%d'\n", node->type);
    exit(1);
}

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_variable_definition(
        node->scope, node
    );
    return node;
}

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_function_definition(
        node->scope, node
    );
    return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
    AST_T* vdef = scope_get_variable_definition(
        node->scope,
        node->variable_name
    );


    if(vdef != (void*)0)
    {
        return visitor_visit(visitor, vdef->variable_definition_value);
    }

    printf("Undefined varibale `%s`\n", node->variable_name);
    exit(1);
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    printf("Makes it here");
    if(strcmp(node->function_call_name, "print") == 0)
    {
        return builtin_function_print(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    AST_T* fdef = scope_get_function_definition(
        node->scope,
        node->function_call_name
    );
    

    if(fdef != (void*)0)
    {
        return visitor_visit(visitor, fdef->function_definition_body);
    }

    printf("Undefined method `%s`\n");
    exit(1);
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->compound_size; i++)
    {
        //printf("%d\n", i)
        visitor_visit(visitor, node->compound_value[i]);
    }

    return init_ast(AST_NOOP);
}