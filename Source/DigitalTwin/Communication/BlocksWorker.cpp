#include "BlocksWorker.h"



FBlocksWorker::FBlocksWorker(const FString &IP, const FString &Topic) : bRunning(false), IP_Interface(IP){
  Thread  = FRunnableThread::Create(this, TEXT("Subscriber : %s"));
}

FBlocksWorker::~FBlocksWorker() {
  if (Thread) {
    // Kill() is a blocking call, it waits for the thread to finish.
    // Hopefully that doesn't take too long
    Thread->Kill();
    delete Thread;
  }
}

bool FBlocksWorker::Init() {
  zmq::context_t Ctx{1};
  Socket = new zmq::socket_t(Ctx, ZMQ_REQ);
  //Socket->set(zmq::sockopt::rcvtimeo, TIMEOUT_TIME);
  //Socket->connect(std::string(TCHAR_TO_UTF8(*IP_Interface)));
  return true;
}

uint32 FBlocksWorker::Run() {

  if(Socket == nullptr)
    return -1;
  
  while (bRunning) {
    
  }
  return 0;
}

void FBlocksWorker::Stop() {
  
}

void FBlocksWorker::Exit() {
  
}

void FBlocksWorker::SendBlocksRequest(const FString Message) {
  
}
