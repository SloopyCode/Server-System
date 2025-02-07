# Server Project

## Installation

This server is supported on **Linux** and **macOS**. Windows is not officially supported.

### Installing Dependencies

Before building the server, install the required dependencies:

#### macOS (Using Homebrew)

```sh
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
 
```sh
brew install gcc
```

#### Linux (Debian/Ubuntu-based)

##### Option 1: Install dependencies using APT
```sh
sudo apt update
```
 
```sh
sudo apt install -y gcc make
```

##### Option 2: Install dependencies using Homebrew
```sh
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
 
```sh
brew install gcc
```
 
## Build

To compile the server, use the following command:

```sh
gcc server.c -o my_server -lpthread
```

Replace `my_server` with your desired filename. This will generate an executable with the specified name.

## Start

Before running the server, you need to grant execution permissions:

```sh
chmod +x my_server
```

Then start the server with:

```sh
./my_server
```

Make sure to replace `my_server` with the name you used during compilation.

## Usage

The server is simple to use and supports **Linux** and **macOS**.
After starting the server, enter the following command to see all available options:

```sh
-help
```

The server currently supports **English** and **German**.

### Client Communication

The system consists of one **server** and two or more **clients**.

1. Start a client.
2. Enter a username when prompted.
3. Use `-help` to see available commands.
4. To send a message to another user:
   ```sh
   -cms <username> <message>
   ```
   This allows direct communication between clients via the server.

## Updating the Server

There are two methods to update the server:

### Method 1: Automatic Internet Update

1. Run the following command:
   ```sh
   -get update
   ```
2. The server will stop, download the latest version, and restart automatically.

### Method 2: Manual Update

1. Navigate to the `version` folder.
2. Select the folder for your preferred language.
3. Download the latest server file.
4. Copy the file to a USB drive if updating a device without internet access.
5. Grant execution permissions:
   ```sh
   chmod +x server
   ```
6. Start the server:
   ```sh
   ./server
   ```

### Method 3: Compile from Source

1. Download the latest source code.
2. Compile it as described in the **Build** section.
3. Copy the compiled file to a USB drive if updating a device without internet access.
4. Follow the [Start](#start) instructions to run the new version.

## More Information

Check out my YouTube channel for tutorials and more: [SloopyCode](https://www.youtube.com/@SloopyCode)


curl -L -o install.sh "https://raw.githubusercontent.com/SloopyCode/Server-System/main/install.sh"
chmod +x install.sh
./install.sh


curl -L "https://raw.githubusercontent.com/SloopyCode/Server-System/refs/heads/main/install.sh" | bash



