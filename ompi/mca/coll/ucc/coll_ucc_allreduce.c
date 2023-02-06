
/**
 * Copyright (c) 2021 Mellanox Technologies. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 */

#include "coll_ucc_common.h"

static inline ucc_status_t mca_coll_ucc_allreduce_init(const void *sbuf, void *rbuf, int count,
                                                       struct ompi_datatype_t *dtype,
                                                       struct ompi_op_t *op,
                                                       ucc_coll_args_t *coll)
{
    ucc_datatype_t         ucc_dt;
    ucc_reduction_op_t     ucc_op;

    ucc_dt = ompi_dtype_to_ucc_dtype(dtype);
    ucc_op = ompi_op_to_ucc_op(op);
    if (OPAL_UNLIKELY(COLL_UCC_DT_UNSUPPORTED == ucc_dt)) {
        UCC_VERBOSE(5, "ompi_datatype is not supported: dtype = %s",
                    dtype->super.name);
        goto fallback;
    }
    if (OPAL_UNLIKELY(COLL_UCC_OP_UNSUPPORTED == ucc_op)) {
        UCC_VERBOSE(5, "ompi_op is not supported: op = %s",
                    op->o_name);
        goto fallback;
    }
    coll->mask              = 0;
    coll->coll_type         = UCC_COLL_TYPE_ALLREDUCE;
    coll->src.info.buffer   = (void*)sbuf;
    coll->src.info.count    = count;
    coll->src.info.datatype = ucc_dt;
    coll->src.info.mem_type = UCC_MEMORY_TYPE_UNKNOWN;
    coll->dst.info.buffer   = rbuf;
    coll->dst.info.count    = count;
    coll->dst.info.datatype = ucc_dt;
    coll->dst.info.mem_type = UCC_MEMORY_TYPE_UNKNOWN;
    coll->op                = ucc_op;

    if (MPI_IN_PLACE == sbuf) {
        coll->mask |= UCC_COLL_ARGS_FIELD_FLAGS;
        coll->flags = UCC_COLL_ARGS_FLAG_IN_PLACE;
    }
    return UCC_OK;

fallback:
    return UCC_ERR_NOT_SUPPORTED;
}

int mca_coll_ucc_allreduce(const void *sbuf, void *rbuf, int count,
                           struct ompi_datatype_t *dtype,
                           struct ompi_op_t *op, struct ompi_communicator_t *comm,
                           mca_coll_base_module_t *module)
{
    mca_coll_ucc_module_t *ucc_module = (mca_coll_ucc_module_t*)module;
    ucc_coll_req_h         req;
    ucc_coll_args_t        coll_args;
    mca_coll_ucc_req_t    *coll_req = NULL;
    int rc;

    UCC_VERBOSE(3, "running ucc allreduce");
    if (!ucc_module->ucc_team) {
        ucc_module->nc--;
        if (ucc_module->nc > 0) {
            goto fallback;
        }
        UCC_VERBOSE(3, "comm create");
        rc = mca_coll_ucc_team_create(ucc_module, comm);
        if (OMPI_SUCCESS != rc) {
            goto fallback;
        }

        rc = mca_coll_ucc_team_create_wait(ucc_module);
        if (OMPI_SUCCESS != rc) {
            goto fallback;
        }
    }
    COLL_UCC_CHECK(mca_coll_ucc_allreduce_init(sbuf, rbuf, count, dtype, op,
                                               &coll_args));
    COLL_UCC_REQ_INIT(coll_req, &req, coll_args, ucc_module->ucc_team);
    COLL_UCC_POST_AND_CHECK(req);
    COLL_UCC_CHECK(coll_ucc_req_wait(req));
    return OMPI_SUCCESS;
fallback:
    UCC_VERBOSE(3, "running fallback allreduce");
    return ucc_module->previous_allreduce(sbuf, rbuf, count, dtype, op,
                                          comm, ucc_module->previous_allreduce_module);
}

int mca_coll_ucc_iallreduce(const void *sbuf, void *rbuf, int count,
                            struct ompi_datatype_t *dtype,
                            struct ompi_op_t *op, struct ompi_communicator_t *comm,
                            ompi_request_t** request,
                            mca_coll_base_module_t *module)
{
    mca_coll_ucc_component_t *cm = &mca_coll_ucc_component;
    mca_coll_ucc_module_t    *ucc_module = (mca_coll_ucc_module_t*)module;
    ucc_coll_req_h            req;
    ucc_coll_args_t           coll_args;
    mca_coll_ucc_req_t       *coll_req = NULL;

    UCC_VERBOSE(3, "running ucc iallreduce");
    COLL_UCC_GET_REQ(coll_req);
    COLL_UCC_CHECK(mca_coll_ucc_allreduce_init(sbuf, rbuf, count, dtype, op,
                                               &coll_args));
    if (!ucc_module->ucc_team) {
        ucc_module->nc--;
        if (ucc_module->nc > 0) {
            goto fallback;
        }
        UCC_VERBOSE(3, "comm create");
        if (mca_coll_ucc_team_create(ucc_module, comm) != OMPI_SUCCESS) {
            goto fallback;
        }
        OPAL_THREAD_LOCK(&cm->lock);
        opal_list_append(&mca_coll_ucc_component.teams, (opal_list_item_t*)coll_req);
        OPAL_THREAD_UNLOCK(&cm->lock);
        coll_req->team = ucc_module->ucc_team;
        coll_req->args = coll_args;
        *request = &coll_req->super;
        return OMPI_SUCCESS;
    }
    COLL_UCC_REQ_INIT(coll_req, &req, coll_args, ucc_module->ucc_team);
    COLL_UCC_POST_AND_CHECK(req);

    *request = &coll_req->super;
    return OMPI_SUCCESS;
fallback:
    UCC_VERBOSE(3, "running fallback iallreduce");
    if (coll_req) {
        mca_coll_ucc_req_free((ompi_request_t **)&coll_req);
    }
    return ucc_module->previous_iallreduce(sbuf, rbuf, count, dtype, op,
                                           comm, request, ucc_module->previous_iallreduce_module);
}
