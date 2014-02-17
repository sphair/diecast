#pragma once

// Client to Server (CTS) Events

#define CTS_PING						"Ping"					// (int time)

#define CTS_LOGIN						"Login"					// (string playerName, string password)

#define CTS_OBJECT_EVENT				"ObjectEvent"			// (int gameObjectId, string eventName, [marshalledEvent]...)

// Server to Client (STC) Events

#define STC_PONG						"Pong"					// (int time)
#define STC_MOTD						"Motd"					// (string html)

#define STC_LOGIN_SUCCESS				"LoginSuccess"			// ()
#define STC_LOGIN_FAIL					"LoginFail"				// (string reason)
#define STC_LOGOUT						"LoggedOut"				// (string reason)

#define STC_TEXT_MESSAGE				"TextMessage"			// (string message)
#define STC_CHAT_MESSAGE				"ChatMessage"			// (string channel, string sender, string message)

#define STC_OBJECT_CREATE				"ObjectCreate"			// (int gameObjectId, 
																//  int parentGameObjectId, 
																//  string gameObjectType, string gameObjectName, 
																//  unsigned int componentCount, 
																//      [string componentType, string componentName, 
																//		 unsigned int propertyCount, 
																//			[int propertyTypeId, string propertyName, string propertyValue]], 
																//  unsigned int propertyCount, 
																//		[int propertyTypeId, string propertyName, string propertyValue])
#define STC_OBJECT_ATTACH				"ObjectAttach"			// (int gameObjectId, int parentGameObjectId)
#define STC_OBJECT_DESTROY				"ObjectDestroy"			// (int gameObjectId, int parentGameObjectId)
#define STC_OBJECT_DETACH				"ObjectDetach"			// (int gameObjectId, int parentGameObjectId)
#define STC_OBJECT_UPDATE				"ObjectUpdate"			// (int gameObjectId, unsigned int propertyCount, [int propertyTypeId, string propertyName, string propertyValue]])
#define STC_OBJECT_EVENT				"ObjectEvent"			// (int gameObjectId, string eventName, [marshalledEvent]...)
