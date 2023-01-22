// Main Class for the Aquaiot project

public with sharing class IOTController{
    
    // Generate IOT Data from Platform Event
    public static void saveData(List<IOTEvent__e> iotPlatformEvents){
        List<IOT_Data__c> listData = new List<IOT_Data__c>();
        Map<String, Integer> dMap = new Map<String, Integer>();
        for(IOTEvent__e ev : iotPlatformEvents){
            if(String.isNotBlank(ev.Payload__c)){
                IOT_Data__c data = new IOT_Data__c();
                dMap.clear();
                for(String each : ev.Payload__c.split('__')){
                    System.debug('Each is=>' + each);
                    String field = each.split('-')[0] + '__c';
                    Integer value = Integer.valueOf(each.split('-')[1]);
                    data.put(field, value);
                    dMap.put(each.split('-')[0], value);
                }
                
                data.Payload__c = JSON.serializePretty(dMap);
                
                
                /*
                Map<String, Object> dataMap = (Map<String, Object>)JSON.deserializeUntyped(data.Payload__c);
                System.debug('data=>' + data);
                for(String each : dataMap.keySet()){
                    String field = each + '__c';
                    String value = (String)dataMap.get(each);
                    data.put(field, value);
                }
                */
                listData.add(data);
            }
        }
        insert listData;
    }
    
}
