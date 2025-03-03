/**
 *  Copyright (C) 2022 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @file CallValidator.cpp
 * @author: kyonGuo
 * @date 2023/4/18
 */

#include "CallValidator.h"
using namespace bcos;
std::pair<bool, bytes> bcos::rpc::CallValidator::verify(
    std::string_view _to, std::string_view _data, std::string_view _sign)
{
    // trim hex prefix
    auto trimHex = [](std::string_view str, std::string_view prefix = "0x") {
        if (str.starts_with(prefix)) [[unlikely]]
        {
            str.remove_prefix(prefix.size());
        }
        return str;
    };
    auto to = trimHex(_to);
    auto data = trimHex(_data);
    auto sign = trimHex(_sign);

    auto hashImpl = m_cryptoSuite->hashImpl();
    crypto::HashType hash;
    auto hasher = hashImpl->hasher();
    // Note: use fromHex because java sdk hash the raw data
    hasher.update(bcos::fromHex(to));
    hasher.update(bcos::fromHex(data));
    hasher.final(hash);
    bcos::bytes signBytes = bcos::fromHex(sign);
    try
    {
        return m_cryptoSuite->signatureImpl()->recoverAddress(
            *hashImpl, hash, bcos::ref(signBytes));
    }
    catch (...)
    {
        return {false, {}};
    }
}
