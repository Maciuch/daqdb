/**
 * Copyright 2017-2018 Intel Corporation.
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you (Intel OBL Internal Use License).
 * Unless the License provides otherwise, you may not use, modify, copy,
 * publish, distribute, disclose or transmit this software or the related
 * documents without Intel's prior written permission.
 *
 * This software and the related documents are provided as is, with no
 * express or implied warranties, other than those that are expressly
 * stated in the License.
 */

#include "FogKV/KVStoreBase.h"

#include <assert.h>
#include <cstring>
#include <limits>

// ![key_struct]

struct Key {
    Key() = default;
    Key(uint64_t e, uint16_t s, uint16_t r)
        : event_id(e), subdetector_id(s), run_id(r) {}
    uint64_t event_id;
    uint16_t subdetector_id;
    uint16_t run_id;
};

// ![key_struct]

int KVStoreBaseExample() {
    // Open KV store
    //! [open]
    FogKV::Options options;

    // Configure key structure
    options.Key.field(0, sizeof(Key::event_id), true); // primary key
    options.Key.field(1, sizeof(Key::subdetector_id));
    options.Key.field(2, sizeof(Key::run_id));

    FogKV::KVStoreBase *kvs;
    try {
        kvs = FogKV::KVStoreBase::Open(options);
    } catch (FogKV::OperationFailedException &e) {
        // error
        // e.status()
    }

    // success

    //! [open]

    //! [put]
    try {
        FogKV::Key key = kvs->AllocKey();

        // Fill the key
        Key *keyp = reinterpret_cast<Key *>(key.data());
        keyp->subdetector_id = 1;
        keyp->run_id = 2;
        keyp->event_id = 3;

        FogKV::Value value = kvs->Alloc(key, 1024);

        // Fil the value buffer
        std::memset(value.data(), 0, value.size());

        // Put operation, transfer ownership of key and value buffers to library
        kvs->Put(std::move(key), std::move(value));
    } catch (...) {
        // error
    }
    //! [put]

    //! [put_async]
    try {
        FogKV::Key key = kvs->AllocKey();

        // Fill the key
        Key *keyp = reinterpret_cast<Key *>(key.data());
        keyp->subdetector_id = 1;
        keyp->run_id = 2;
        keyp->event_id = 3;

        FogKV::Value value = kvs->Alloc(key, 1024);

        // Fill the value buffer
        std::memset(value.data(), 0, value.size());

        // Asynchronous Put operation, transfer ownership of key and value
        // buffers to library
        kvs->PutAsync(std::move(key), std::move(value),
                      [&](FogKV::KVStoreBase *kvs, FogKV::Status status,
                          const char *key, size_t keySize, const char *value,
                          size_t valueSize) {
                          if (!status.ok()) {
                              // error
                              return;
                          }

                      });
    } catch (...) {
        // error
    }
    //! [put_async]

    //! [get]
    try {

        FogKV::Key key = kvs->AllocKey();

        // Fill the key
        Key *keyp = reinterpret_cast<Key *>(key.data());
        keyp->subdetector_id = 1;
        keyp->run_id = 2;
        keyp->event_id = 3;

        // Get operation, the caller becomes the owner of a local copy
        // of the value. The caller must free both key and value buffers by
        // itself, or
        // transfer the ownership in another call.
        FogKV::Value value;
        try {
            value = kvs->Get(key);
        } catch (...) {
            // error
            kvs->Free(std::move(key));
        }

        // success, process the data and free the buffers
        kvs->Free(std::move(key));
        kvs->Free(std::move(value));
    } catch (...) {
        // error
    }
    //! [get]

    //! [get_async]
    try {
        FogKV::Key key = kvs->AllocKey();

        // Fill the key
        Key *keyp = reinterpret_cast<Key *>(key.data());
        keyp->subdetector_id = 1;
        keyp->run_id = 2;
        keyp->event_id = 3;

        try {
            kvs->GetAsync(key,
                          [&](FogKV::KVStoreBase *kvs, FogKV::Status status,
                              const char *key, size_t keySize,
                              const char *value, size_t valueSize) {

                              if (!status.ok()) {
                                  // error
                                  return;
                              }

                              // process value

                              // free the value buffer
                          });
        } catch (FogKV::OperationFailedException exc) {
            // error, status in:
            // exc.status();
            kvs->Free(std::move(key));
        }
    } catch (...) {
        // error
    }
    //! [get_async]

    //! [get_any]
    try {

        FogKV::GetOptions getOptions;
        getOptions.Attr = FogKV::READY;
        getOptions.NewAttr = FogKV::LOCKED | FogKV::READY;

        // get and lock any primary key which is in unlocked state
        FogKV::Key keyBuff = kvs->GetAny(getOptions);
        Key *key = reinterpret_cast<Key *>(keyBuff.data());

        Key keyBeg(key->event_id, std::numeric_limits<uint16_t>::min(),
                   std::numeric_limits<uint16_t>::min());

        Key keyEnd(key->event_id, std::numeric_limits<uint16_t>::max(),
                   std::numeric_limits<uint16_t>::max());

        std::vector<FogKV::KVPair> range = kvs->GetRange(
            FogKV::Key(reinterpret_cast<char *>(&keyBeg), sizeof(keyBeg)),
            FogKV::Key(reinterpret_cast<char *>(&keyEnd), sizeof(keyEnd)));

        for (auto kv : range) {
            // or unlock and mark the primary key as ready
            kvs->Update(kv.key(), FogKV::READY);
        }

    } catch (...) {
        // error
    }
    //! [get_any]
}

int main() {}
