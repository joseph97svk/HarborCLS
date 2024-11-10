# HarborCLS

## Project Overview ##
HarborCLS is a C++ project that provides a implementation of a small extendable and flexible framework capable of handling client, server, and distributed system tasks through custom developer implemented protocols. It leverages modern C++ standards and several third-party libraries to deliver robust functionality.

## Pre-requisites ##

### Installing latest version of CMake ###
```bash
sudo apt-get install snapd
sudo snap install cmake --classic
```

### Installing C++23 and gcc 13 ###
https://developernote.com/2023/08/installing-gcc13-on-ubuntu-22-04/

```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-13 g++-13
ll /usr/bin/gcc-13
ll /usr/bin/g++-13
update-alternatives --display gcc
ll /etc/alternatives/g*
sudo update-alternatives --remove-all gcc
sudo update-alternatives --remove-all g++
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 10 --slave /usr/bin/g++ g++ /usr/bin/g++-13
g++ --version
gcc --version
```

### Installing SSL Support ###
```bash
sudo apt-get install libssl-dev
```

## Third party libraries used ##
### Json reader: Nlohmann's Json ###
https://github.com/nlohmann/json

### Inversion of Control Container: Ybainier's Hypodermic ###
https://github.com/ybainier/Hypodermic
### Logger: spdlog ###
https://github.com/gabime/spdlog

### Dependency Injection: Boost.DI ###
https://github.com/boost-experimental/di

### googletest ###
https://github.com/google/googletest

## Building the Project ##

### Compilation Instructions ###
To compile the project, use the `build.sh` script provided in the root directory of the project. Below are the instructions:

1. **Navigate to the Project Directory**:
   Make sure you are in the root directory of the project:
   ```bash
   cd /path/to/HarborCLS
   ```

2. **Basic Compilation**:
   To compile the project without any sanitizers:
   ```bash
   ./build.sh
   ```

3. **Cleaning the Build Directory**:
   To remove the existing `Build` directory before starting a new build:
   ```bash
   ./build.sh clean
   ```

4. **Compilation with AddressSanitizer (ASAN)**:
   To compile the project with AddressSanitizer enabled for detecting memory errors:
   ```bash
   ./build.sh asan
   ```

5. **Compilation with ThreadSanitizer (TSAN)**:
   To compile the project with ThreadSanitizer enabled for detecting data race conditions:
   ```bash
   ./build.sh tsan
   ```

6. **Help Option**:
   To display help information about the script:
   ```bash
   ./build.sh help
   ```

### Script Details ###
- The `build.sh` script creates the `Build` directory if it does not exist and runs CMake configuration inside it.
- Optional arguments can be passed to enable specific build configurations:
  - **`clean`**: Cleans the `Build` directory.
  - **`asan`**: Builds the project with AddressSanitizer (`-DASAN_BUILD=ON`).
  - **`tsan`**: Builds the project with ThreadSanitizer (`-DTSAN_BUILD=ON`).
- If no argument is provided, the project is built normally without any sanitizers.

### Important Note ###
Ensure the script has execute permissions before running:
```bash
chmod +x build.sh
```

## Logger Usage ##

### How the Logger Works ###
The logger in HarborCLS is designed to be flexible and configurable. It supports different buffering strategies, file management policies, and file rotation policies. The logger can be configured to either always keep the log file open or open it on demand. It can also buffer log messages and rotate log files based on specific policies.

### Using the Logger ###
To use the logger, you need to create an instance of the `Logger` class. Here is an example of how to use the logger:

```cpp
#include "Logger/Logger.hpp"
#include "Logger/LoggingBufferingPolicies/NoBufferingPolicy.hpp"
#include "Logger/LoggingFileManagementPolicies/AlwaysOpenFilePolicy.hpp"
#include "Logger/LogFileRotationPolicies/NoRotationPolicy.hpp"

int main() {
    std::string logFilePath = "log.txt";
    auto bufferingPolicy = std::make_shared<HarborCLS::NoBufferingPolicy>();
    auto fileManagementPolicy = std::make_shared<HarborCLS::AlwaysOpenFilePolicy>();
    auto fileRotationPolicy = std::make_shared<HarborCLS::NoRotationPolicy>();

    HarborCLS::Logger logger(bufferingPolicy, fileManagementPolicy, fileRotationPolicy, logFilePath);

    logger.log("This is a log message.");
    logger.warning("This is a warning message.");
    logger.error("This is an error message.");
    logger.info("This is an info message.");

    return 0;
}
```

### Using the Logger Factory ###
The `LoggerFactory` class can be used to create logger instances with a specific configuration. Here is an example of how to use the factory to produce a logger:

```cpp
#include "Logger/LoggerFactory/LoggerFactory.hpp"
#include "Logger/LoggerConfiguration.hpp"

int main() {
    HarborCLS::LoggerConfiguration config;
    config.sharedLog = true; // Use shared logger

    HarborCLS::LoggerFactory<> factory(config);

    auto logger = factory.createLogger();

    logger->log("This is a log message from the factory.");
    logger->warning("This is a warning message from the factory.");
    logger->error("This is an error message from the factory.");
    logger->info("This is an info message from the factory.");

    return 0;
}
```

In this example, the `LoggerFactory` is used to create a shared logger instance based on the provided configuration.

### Extending the Logger ###
The logger in HarborCLS is designed to be extendable. You can create custom buffering strategies, file management policies, and file rotation policies by extending the provided interfaces.

#### Buffering Policies
To create a custom buffering policy, extend the `ILoggerBufferingPolicy` interface:

```cpp
#include "Logger/LoggingBufferingPolicies/ILoggerBufferingPolicy.hpp"

class CustomBufferingPolicy : public HarborCLS::ILoggerBufferingPolicy {
public:
    std::optional<std::string> buffer(std::string &message) override {
        // Implement custom buffering logic here
        return std::nullopt;
    }
};
```

#### File Management Policies
To create a custom file management policy, extend the `ILoggerFileManagementPolicy` interface:

```cpp
#include "Logger/LoggingFileManagementPolicies/ILoggerFileManagementPolicy.hpp"

class CustomFileManagementPolicy : public HarborCLS::ILoggerFileManagementPolicy {
public:
    std::variant<std::string, std::ofstream> getLogFile(std::string &logFilePath) override {
        // Implement custom file management logic here
        return logFilePath;
    }

    void log(std::string completeLoggingMessage,
             std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
             std::mutex &canWrite) override {
        // Implement custom logging logic here
    }
};
```

#### File Rotation Policies
To create a custom file rotation policy, extend the `ILogFileRotation` interface:

```cpp
#include "Logger/LogFileRotationPolicies/ILogFileRotation.hpp"

class CustomFileRotationPolicy : public HarborCLS::ILogFileRotation {
public:
    void rotateLogFile(std::ofstream &file, std::string &fileName) override {
        // Implement custom file rotation logic here
    }
};
```

By extending these interfaces, you can customize the behavior of the logger to fit your specific needs.

## JSON Reader ##

### How the JSON Reader Works ###
The JSON reader in HarborCLS is designed to handle serialization and deserialization of data using customizable policies. It leverages the `nlohmann::json` library for JSON parsing and provides a flexible framework for defining how data should be processed.

1. **Define Policies**: Create policies by extending `IPolicyOperation` or using `PolicyOperation` to define how specific attributes should be serialized and deserialized.
2. **Add Policies to ParsingPolicy**: Implement the `addAllPolicies` method in a subclass of `ParsingPolicy` to add all necessary policies.
3. **Use JsonHandler**: Instantiate `JsonHandler` with the data or JSON string/path and the custom `ParsingPolicy`. Call `serialize` or `deserialize` to perform the operations.

### Extending the JSON Reader ###
To extend the JSON reader, follow these steps:

1. **Create Custom Policies**:
   - **Buffering Policies**: Extend `ILoggerBufferingPolicy` to create custom buffering strategies.
   - **File Management Policies**: Extend `ILoggerFileManagementPolicy` to manage how log files are handled.
   - **File Rotation Policies**: Extend `ILogFileRotation` to define custom file rotation logic.

2. **Implement Custom ParsingPolicy**:
   - Subclass `ParsingPolicy` and implement the `addAllPolicies` method to add your custom policies.

3. **Use Custom ParsingPolicy in JsonHandler**:
   - Instantiate `JsonHandler` with your custom `ParsingPolicy` to apply the custom serialization and deserialization logic.

### Example Usage ###
Here is an example of how to use the JSON reader:
```cpp
#include "JsonHandler.hpp"
#include "ParsingPolicy.hpp"
#include "PolicyOperation.hpp"

class DetailedParsingPolicy : public HarborCLS::ParsingPolicy {
public:
    void addAllPolicies() override {
        // Add policies for various attributes
        addPolicy("name", std::make_shared<HarborCLS::PolicyOperation<std::string>>());
        addPolicy("age", std::make_shared<HarborCLS::PolicyOperation<int>>());
        addPolicy("email", std::make_shared<HarborCLS::PolicyOperation<std::string>>());
        addPolicy("isActive", std::make_shared<HarborCLS::PolicyOperation<bool>>());
    }
};

int main() {
    std::string jsonString = R"({"name": "John Doe", "age": 30, "email": "john.doe@example.com", "isActive": true})";
    DetailedParsingPolicy policy;
    HarborCLS::JsonHandler handler(jsonString, policy);

    handler.deserialize();

    std::string name = handler.jsonAt<std::string>("name");
    int age = handler.jsonAt<int>("age");
    std::string email = handler.jsonAt<std::string>("email");
    bool isActive = handler.jsonAt<bool>("isActive");

    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "Email: " << email << std::endl;
    std::cout << "Is Active: " << isActive << std::endl;

    return 0;
}
```

In this detailed example, the `addAllPolicies` method is implemented to add policies for various attributes such as `name`, `age`, `email`, and `isActive`. The `JsonHandler` is then used to deserialize a JSON string and retrieve the values of these attributes.


## Dependency Management ##

### How the DependencyManager Works ###
The `DependencyManager` class in HarborCLS is responsible for managing the dependencies of the application using the Hypodermic library. It allows you to register and resolve dependencies in a flexible and modular way.

### Using the DependencyManager ###
To use the `DependencyManager`, you need to create an instance of the class and register your dependencies. Here is an example of how to use the `DependencyManager`:

```cpp
#include "Common/DependenciesManagement/DependencyManager.hpp"
#include "SomeService.hpp"

int main() {
    HarborCLS::DependencyManager dependencyManager;

    // Register a singleton service
    dependencyManager.addSingleton<SomeService>();

    // Create a scoped container
    auto scopedContainer = dependencyManager.createScopedContainer();

    // Resolve the service
    auto someService = scopedContainer->resolve<SomeService>();

    someService->performTask();

    return 0;
}
```

### Methods of DependencyManager ###

#### `addScoped<T>()`
Registers a scoped service of type `T`. Scoped services are created once per scope.

```cpp
dependencyManager.addScoped<SomeService>();
```

#### `addSingleton<T>()`
Registers a singleton service of type `T`. Singleton services are created once and shared across the application.

```cpp
dependencyManager.addSingleton<SomeService>();
```

#### `addTransient<T>()`
Registers a transient service of type `T`. Transient services are created each time they are requested.

```cpp
dependencyManager.addTransient<SomeService>();
```

#### `addInstance<T>(std::shared_ptr<T>& instance)`
Registers an instance of type `T`. This allows you to register a pre-existing instance.

```cpp
auto someServiceInstance = std::make_shared<SomeService>();
dependencyManager.addInstance(someServiceInstance);
```

#### `createScopedContainer()`
Creates a scoped container from the global container. Scoped containers are used to resolve scoped services.

```cpp
auto scopedContainer = dependencyManager.createScopedContainer();
```

#### `getGlobalBuilder()`
Returns the global builder used for registering global services.

```cpp
auto& globalBuilder = dependencyManager.getGlobalBuilder();
```

#### `getScopedBuilder()`
Returns the scoped builder used for registering scoped services.

```cpp
auto& scopedBuilder = dependencyManager.getScopedBuilder();
```

#### `getTaskBuilder()`
Returns the task builder used for registering task-specific services.

```cpp
auto& taskBuilder = dependencyManager.getTaskBuilder();
```
