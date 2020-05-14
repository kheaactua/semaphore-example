#include <iostream>
#include <exception>

#include <boost/program_options.hpp>

#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/permissions.hpp>

auto service_main()
{
    boost::interprocess::named_semaphore sema_run(boost::interprocess::open_only_t(), "my_semaphore");
    std::cout << "Service: wait" << std::endl;
    sema_run.wait();
    std::cout << "Service: continuing" << std::endl;
    std::cout << "Service: done" << std::endl;
}

auto client_main(int const client_uid)
{
    std::cout << "Setting uid = " << client_uid << "\n";

    setuid(client_uid);
    setgid(client_uid);

    std::cout << "Client: UID = " << getuid() << " EUID = " << geteuid() << "\n";

    boost::interprocess::named_semaphore sema_run(boost::interprocess::open_only_t(), "my_semaphore");
    std::cout << "Client: post" << std::endl;
    sema_run.post();
    std::cout << "Client: done" << std::endl;
}

auto main(int argc, char** argv) -> int
{
    namespace po = boost::program_options;

    // clang-format off
    po::options_description desc("Usage");
    desc.add_options()
        ("client-uid,c",   po::value<int>()->default_value(1101),   "UID to use to run the client process")
        ("skip-client,C",  po::bool_switch()->default_value(false), "Do not run the client portion")
        ("skip-service,S", po::bool_switch()->default_value(false), "Do not run the service portion")
    ;
    // clang-format on

    po::variables_map opts;
    po::store(po::parse_command_line(argc, argv, desc), opts);
    try
    {
        po::notify(opts);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    auto client_uid = opts["client-uid"].as<int>();
    auto skip_client = opts["skip-client"].as<bool>();
    auto skip_service = opts["skip-service"].as<bool>();

    // Delete any semaphore that exists with this name
    boost::interprocess::named_semaphore::remove("my_semaphore");

    // Create the semaphore
    std::cout << "Setting unrestricted permissions\n";
    boost::interprocess::permissions perms;
    perms.set_unrestricted();
    perms.set_permissions(0777);
    boost::interprocess::named_semaphore(boost::interprocess::create_only_t(), "my_semaphore", 0, perms);

    auto pid = fork();

    if (0 == pid)
    {
        // Client (child process)

        if (skip_client)
        {
            std::cout << "Skipping client\n";
        }
        else
        {
            client_main(client_uid);
        }
    }
    else
    {
        // Service
        std::cout << "In pid != 0\n";

        if (skip_service)
        {
            std::cout << "Skipping service\n";
        }
        else
        {
            service_main();
        }
    }
}
