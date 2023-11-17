#include "ZeroMqWorker.h"
#define TIMEOUT_TIME 500

FZeroMqWorker::FZeroMqWorker(const TSharedPtr<TQueue<FTrafficData>>& Q,const FString &IP,const FString &Topic) :
	Running(false), IP_Interface(IP), Queue(Q)
{
	TopicStrings.Add(FString(Topic));
	Thread  = FRunnableThread::Create(this, TEXT("Subscriber : %s"));
}

FZeroMqWorker::~FZeroMqWorker() {
	if (Thread) {
		// Kill() is a blocking call, it waits for the thread to finish.
		// Hopefully that doesn't take too long
		Thread->Kill();
		delete Thread;
	}
}

void FZeroMqWorker::HandlePublicTraffic(const std::string &Message_Str) const
{
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FString(Message_Str.c_str()));
	TSharedPtr<FJsonObject> JsonObject;
	const auto Res = FJsonSerializer::Deserialize(JsonReader, JsonObject);
	
	if (Res) {
		const auto Id	    = JsonObject->GetStringField("id"); 
		const auto Heading    = JsonObject->GetNumberField("heading");
		const auto Latitude   = JsonObject->GetNumberField("latitude");
		const auto Longitude  = JsonObject->GetNumberField("longitude");
		const auto Type       = JsonObject->GetStringField("type");
		const auto Color_String     = JsonObject->GetStringField("color");
		const auto Status_String    = JsonObject->GetStringField("Status");

		int64 Time = 0;
		if(JsonObject->TryGetNumberField("time",Time)) {
			Time = FDateTime::Now().ToUnixTimestamp();
		}
		
		FTrafficData Transport;
		Transport.Id        = Id;
		Transport.TimeStamp = Time;
		Transport.Position  = FVector(0.0,Latitude,Longitude);
		Transport.Heading   = Heading;
		Transport.Type      = Type;
		Transport.Color     = FColor::FromHex(Color_String);
		Transport.Radius    = 5.f;
		Transport.Status    = Status_String;
		Queue->Enqueue(Transport);
	}
}

void FZeroMqWorker::HandleImageOther(const std::string& Message_Str) const
{
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FString(Message_Str.c_str()));
	TSharedPtr<FJsonObject> JsonObject;
	const auto Res = FJsonSerializer::Deserialize(JsonReader, JsonObject);
	
	if (Res) {
		const auto Id		= JsonObject->GetStringField("id"); 
		const auto Heading   = JsonObject->GetNumberField("heading");
		const auto Latitude  = JsonObject->GetNumberField("latitude");
		const auto Longitude = JsonObject->GetNumberField("longitude");
			
		FTrafficData Transport;
		Transport.Id       = Id;
		Transport.Position = FVector(0.0,Latitude,Longitude);
		Transport.Heading  = Heading;
		Transport.Color  = FColor::Red;
		Transport.Radius = 10.f;
		Queue->Enqueue(Transport);
	}
}

void FZeroMqWorker::SetTopic(const FString &Topic)
{
	TopicStrings.Add(Topic);
}


uint32 FZeroMqWorker::Run() {
	zmq::context_t Ctx{1};
	
	Socket = new zmq::socket_t(Ctx, ZMQ_SUB);
	Socket->set(zmq::sockopt::rcvtimeo, TIMEOUT_TIME);
	Socket->connect(std::string(TCHAR_TO_UTF8(*IP_Interface)));
	
	SetUpTopics();
	
	if(Socket == nullptr)
		return -1;
	
	while (Running) {
		zmq::message_t     Msg;
		zmq::recv_result_t Rec_Result = Socket->recv(Msg, zmq::recv_flags::none);
		
		if (!Rec_Result.has_value()) continue;
		
		std::string Message_Str = Msg.to_string();
		std::string Topic = Message_Str.substr(0, Message_Str.find(' '));

		const auto Message_Topic = FString(Topic.c_str());

		//TODO: Handle message topic nicer !
		if(Message_Topic == "PublicTraffic") {
			Message_Str.erase(0, Topic.size() + 1);
			HandlePublicTraffic(Message_Str);
		}
	}
	Socket->close();
	return 0;
}

bool FZeroMqWorker::Init() {
	int Major, Minor, Patch = 0;
	zmq::version(&Major,&Minor,&Patch);
	UE_LOG(LogTemp, Warning, TEXT("ZeroMQ version: v%d.%d.%d"), Major, Minor, Patch);
	Running = true;
	return true;
}

void FZeroMqWorker::Stop() {
	UE_LOG(LogTemp, Warning, TEXT("FZeroMqWorker thread is stopping!"));
	Running = false;
}
void FZeroMqWorker::Exit() {
	UE_LOG(LogTemp, Warning, TEXT("FZeroMqWorker thread is Exiting!"));
}


void FZeroMqWorker::SetQueue(const TSharedPtr<TQueue<FTrafficData> > &Q) {
	Queue = Q;
}

void FZeroMqWorker::SetUpTopics() {
	for(const auto& Topic : TopicStrings ) {
		Socket->set(zmq::sockopt::subscribe, std::string(TCHAR_TO_UTF8(*Topic)));
	}
}