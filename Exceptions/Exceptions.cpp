#include "IExceptions.h"
#include <iostream>

using namespace std;

const string FilePathException::messages::rootFileNotFound = "RootFolder not found";
const string FilePathException::messages::modelNotFound = "Model not found";

const string RecognitionException::messages::microphoneInitializeError = "Failed to initialize microphone!";
const string RecognitionException::messages::recordFail = "Failed to start recording!";
const string RecognitionException::messages::modelLoadError = "Failed to load Vosk model.";
const string RecognitionException::messages::recognizerFail = "Failed to create recognizer.";

const string AudioException::messages::audioInitializeError = "Audio Player initializing error";
const string AudioException::messages::audioPlayError = "Fail to play response.wav";