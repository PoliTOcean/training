// async_publish.cpp
//
// This is a Paho MQTT C++ client, sample application.
//
// It's an example of how to send messages as an MQTT publisher using the
// C++ asynchronous client interface.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker
//  - Publishing messages
//  - Last will and testament
//  - Using asynchronous tokens
//  - Implementing callbacks and action listeners
//

/*******************************************************************************
 * Copyright (c) 2013-2017 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

#include "mqtt/async_client.h"
#include "threadsafe/queue.h"

#include "spdlog/spdlog.h"

using namespace std;

const std::string DFLT_SERVER_ADDRESS{"tcp://localhost:1883"};
const std::string DFLT_CLIENT_ID{"async_publish"};
const int N_PUBLISHER{3};

const string TOPIC{"hello"};

const auto TIMEOUT = std::chrono::seconds(10);

/**
 * A callback class for use with the main MQTT client.
 */
class callback : public virtual mqtt::callback
{
public:
    void connection_lost(const string &cause) override
    {
        spdlog::warn("Connection lost with cause {}", (!cause.empty() ? cause : ""));
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override
    {
        spdlog::warn("Connection lost with cause {}", (tok ? tok->get_message_id() : -1));
    }
};

int main(int argc, char *argv[])
{
    string address = (argc > 1) ? string(argv[1]) : DFLT_SERVER_ADDRESS,
           clientID = (argc > 2) ? string(argv[2]) : DFLT_CLIENT_ID;

    threadsafe::queue<std::string> q;
    std::vector<std::thread> threadpool;

    std::thread mainThread([&q]() {
        std::string message = "";
        while (message != "END")
        {
            cin >> message;
            q.push(message);
        }
    });

    for (int i = 0; i < N_PUBLISHER; i++)
        threadpool.emplace_back([address, clientID, i, &q] {
            spdlog::info("Initializing client {} for server {}...", i, address);

            mqtt::async_client client(address, std::to_string(i));
            callback cb;
            client.set_callback(cb);

            mqtt::connect_options conopts;

            spdlog::info("\t...OK for client {}", i);

            spdlog::info("Connecting client {}...", i);

            try
            {
                mqtt::token_ptr conntok = client.connect(conopts);
                spdlog::info("Waiting for the connection for client {}...", i);
                conntok->wait();
                spdlog::info("\t..OK for client {}", i);

                while (1)
                    if (!q.empty())
                    {
                        std::optional<std::string> message = q.pop();
                        if (message.has_value())
                        {
                            spdlog::info("Sending message...");
                            mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, message.value());
                            client.publish(pubmsg)->wait_for(TIMEOUT);
                            spdlog::info("\t...OK");
                        }
                    }
            }
            catch (const mqtt::exception &e)
            {
                spdlog::critical("Error {}", e.what());
            }
        });

    for (auto &t : threadpool)
        t.detach();

    mainThread.join();

    return 0;
}
