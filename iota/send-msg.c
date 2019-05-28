/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "send-common.h"

int mam_send_message(char *host, int port, char *seed, char *payload, size_t payload_size, bool last_packet) {
  mam_api_t api;
  bundle_transactions_t *bundle = NULL;
  tryte_t channel_id[MAM_CHANNEL_ID_SIZE];
  retcode_t ret = RC_OK;

  // Loading or creating MAM API
  if ((ret = mam_api_load(MAM_FILE, &api)) == RC_UTILS_FAILED_TO_OPEN_FILE) {
    if ((ret = mam_api_init(&api, (tryte_t *)seed)) != RC_OK) {
      fprintf(stderr, "mam_api_init failed with err %d\n", ret);
      return EXIT_FAILURE;
    }
  } else if (ret != RC_OK) {
    fprintf(stderr, "mam_api_load failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Creating channel
  if ((ret = mam_example_create_channel(&api, channel_id)) != RC_OK) {
    fprintf(stderr, "mam_example_create_channel failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  bundle_transactions_new(&bundle);

  {
    trit_t msg_id[MAM_MSG_ID_SIZE];

    // Writing header to bundle
    if ((ret = mam_example_write_header_on_channel(&api, channel_id, bundle, msg_id)) != RC_OK) {
      fprintf(stderr, "mam_example_write_header failed with err %d\n", ret);
      return EXIT_FAILURE;
    }

    // if (mam_channel_num_remaining_sks(channel) == 0) {
    // TODO
    // - remove old ch
    // - create new ch
    // - add ch via `mam_api_add_channel`

    //   return RC_OK;
    // }

    if ((ret = mam_example_write_packet(&api, bundle, payload, payload_size, msg_id, last_packet)) != RC_OK) {
      fprintf(stderr, "mam_example_write_packet failed with err %d\n", ret);
      return EXIT_FAILURE;
    }
  }

  // Sending bundle
  if ((ret = send_bundle(host, port, bundle)) != RC_OK) {
    fprintf(stderr, "send_bundle failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Saving and destroying MAM API
  if ((ret = mam_api_save(&api, MAM_FILE)) != RC_OK) {
    fprintf(stderr, "mam_api_save failed with err %d\n", ret);
  }
  if ((ret = mam_api_destroy(&api)) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Cleanup
  { bundle_transactions_free(&bundle); }

  return EXIT_SUCCESS;
}
