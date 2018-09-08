/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */
#include "example_vm.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR(x) #x

#if !defined(PROJECT_VERSION)
#define PROJECT_VERSION 0.0.0
#endif

struct example_vm
{
    struct evmc_instance instance;
    int verbose;
    evmc_trace_callback trace_callback;
    struct evmc_tracer_context* tracer_context;
};

static void destroy(struct evmc_instance* evm)
{
    free(evm);
}

static evmc_capabilities_flagset get_capabilities(struct evmc_instance* vm)
{
    (void)vm;
    return EVMC_CAPABILITY_EVM1 | EVMC_CAPABILITY_EWASM;
}

/// Example options.
///
/// VMs are allowed to omit this function implementation.
static enum evmc_set_option_result set_option(struct evmc_instance* instance,
                                              const char* name,
                                              const char* value)
{
    struct example_vm* vm = (struct example_vm*)instance;
    if (strcmp(name, "verbose") == 0)
    {
        if (!value)
            return EVMC_SET_OPTION_INVALID_VALUE;

        char* end = NULL;
        long int v = strtol(value, &end, 0);
        if (end == value)  // Parsing the value failed.
            return EVMC_SET_OPTION_INVALID_VALUE;
        if (v > 9 || v < -1)  // Not in the valid range.
            return EVMC_SET_OPTION_INVALID_VALUE;
        vm->verbose = (int)v;
        return EVMC_SET_OPTION_SUCCESS;
    }

    return EVMC_SET_OPTION_INVALID_NAME;
}

static void release_result(struct evmc_result const* result)
{
    (void)result;
}

static void free_result_output_data(struct evmc_result const* result)
{
    free((uint8_t*)result->output_data);
}

static struct evmc_result execute(struct evmc_instance* instance,
                                  struct evmc_context* context,
                                  enum evmc_revision rev,
                                  const struct evmc_message* msg,
                                  const uint8_t* code,
                                  size_t code_size)
{
    struct evmc_result ret = {.status_code = EVMC_INTERNAL_ERROR};
    if (code_size == 0)
    {
        // In case of empty code return a fancy error message.
        const char* error = rev == EVMC_BYZANTIUM ? "Welcome to Byzantium!" : "Hello Ethereum!";
        ret.output_data = (const uint8_t*)error;
        ret.output_size = strlen(error);
        ret.status_code = EVMC_FAILURE;
        ret.release = NULL;  // We don't need to release the constant messages.
        return ret;
    }

    struct example_vm* vm = (struct example_vm*)instance;

    // Simulate executing by checking for some code patterns.
    // Solidity inline assembly is used in the examples instead of EVM bytecode.

    // Assembly: `{ mstore(0, address()) return(0, msize()) }`.
    const char return_address[] = "\x30\x60\x00\x52\x59\x60\x00\xf3";

    // Assembly: `{ sstore(0, add(sload(0), 1)) }`
    const char counter[] = "\x60\x01\x60\x00\x54\x01\x60\x00\x55";

    if (code_size == strlen(return_address) &&
        strncmp((const char*)code, return_address, code_size) == 0)
    {
        static const size_t address_size = sizeof(msg->destination);
        uint8_t* output_data = (uint8_t*)malloc(address_size);
        if (!output_data)
        {
            // malloc failed, report internal error.
            ret.status_code = EVMC_INTERNAL_ERROR;
            return ret;
        }
        memcpy(output_data, &msg->destination, address_size);
        ret.status_code = EVMC_SUCCESS;
        ret.output_data = output_data;
        ret.output_size = address_size;
        ret.release = &free_result_output_data;
        return ret;
    }
    else if (code_size == strlen(counter) && strncmp((const char*)code, counter, code_size) == 0)
    {
        const evmc_bytes32 key = {{0}};
        evmc_bytes32 value = context->host->get_storage(context, &msg->destination, &key);
        value.bytes[31]++;
        context->host->set_storage(context, &msg->destination, &key, &value);
        ret.status_code = EVMC_SUCCESS;
        return ret;
    }

    ret.release = release_result;
    ret.status_code = EVMC_FAILURE;
    ret.gas_left = 0;

    if (vm->verbose)
        printf("Execution done.\n");

    return ret;
}

static void set_tracer(struct evmc_instance* instance,
                       evmc_trace_callback callback,
                       struct evmc_tracer_context* context)
{
    struct example_vm* vm = (struct example_vm*)instance;
    vm->trace_callback = callback;
    vm->tracer_context = context;
}

struct evmc_instance* evmc_create_example_vm()
{
    struct evmc_instance init = {
        .abi_version = EVMC_ABI_VERSION,
        .name = "example_vm",
        .version = STR(PROJECT_VERSION),
        .destroy = destroy,
        .execute = execute,
        .get_capabilites = get_capabilities,
        .set_option = set_option,
        .set_tracer = set_tracer,
    };
    struct example_vm* vm = calloc(1, sizeof(struct example_vm));
    struct evmc_instance* interface = &vm->instance;
    memcpy(interface, &init, sizeof(init));
    return interface;
}
