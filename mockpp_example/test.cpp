#include "stdafx.h"

class _declspec(novtable) IHttpRestClient
{
public:
	virtual ~IHttpRestClient() = 0 {}
	virtual std::string getResourcesAsJsonString(const std::string &uri) const = 0;
};

struct DeviceResource
{
	DeviceResource(const char *domain, const char *netbios) : domainName(domain), netbiosName(netbios) {}
	std::string domainName;
	std::string netbiosName;
};

class DeviceResourceNetworkRepository
{
public:
	typedef std::vector<DeviceResource> devices_container_t;
	DeviceResourceNetworkRepository(const IHttpRestClient &restClient) : _restClient(restClient) {}

	devices_container_t getAll(const std::string &resourcePath) 
	{ 
		std::string &result = _restClient.getResourcesAsJsonString(resourcePath);

		rapidjson::Document document;
		devices_container_t devices;

		if (!document.Parse<0>(result.c_str()).HasParseError() && document.IsArray())
		{
			for (rapidjson::SizeType idx = 0; idx < document.Size(); ++idx)
			{
				const rapidjson::Value &value = document[idx];
				const char *domainName = value["domainName"].GetString();
				const char *netbiosName = value ["netbiosName"].GetString();
				devices.push_back(DeviceResource(domainName, netbiosName));
			}
		}

		return devices;
	}

private:
	const IHttpRestClient &_restClient;
};

USING_NAMESPACE_MOCKPP

class MockHttpRestClient : public IHttpRestClient, public MOCKPP_NS::MockObject
{
public:
	MockHttpRestClient() 
		: MOCKPP_NS::MockObject(MOCKPP_PCHAR("MockHttpClient"), 0)
		, getResourcesAsJsonString_output(MOCKPP_PCHAR("MockHttpClient/getResourcesAsJsonString_output"), this)
		, getResourcesAsJsonString_input(MOCKPP_PCHAR("MockHttpClient/getResourcesAsJsonString_input"), this)
	{
	}

	virtual std::string getResourcesAsJsonString(const std::string &uri) const
	{
		getResourcesAsJsonString_input.addActual(uri);
		return getResourcesAsJsonString_output.nextReturnObject();
	}

	mutable MOCKPP_NS::ReturnObjectList<std::string>    getResourcesAsJsonString_output;
	mutable MOCKPP_NS::ConstraintList<std::string>		getResourcesAsJsonString_input;
};

TEST(DeviceResourceNetworkRepository, Positive)
{
	char *html = "[{\"domainName\" : \"1E\", \"netbiosName\" : \"testMachine\"}]";
	MockHttpRestClient mockHttpClient;
	DeviceResourceNetworkRepository repository(mockHttpClient);

	mockHttpClient.getResourcesAsJsonString_input.addExpected(eq<std::string>("http://example.net/Devices"));
	mockHttpClient.getResourcesAsJsonString_output.addObjectToReturn(std::string(html));

	std::vector<DeviceResource> resources = repository.getAll("http://example.net/Devices");
	
	mockHttpClient.verify();
	
	EXPECT_EQ(1, resources.size());
	EXPECT_STREQ("1E", resources[0].domainName.c_str());
}

TEST(DeviceResourceNetworkRepository, Negative) 
{
	char *html = "";

	MockHttpRestClient mockHttpClient;
	DeviceResourceNetworkRepository repository(mockHttpClient);

	mockHttpClient.getResourcesAsJsonString_input.addExpected(eq<std::string>("http://example.net/Devices"));
	mockHttpClient.getResourcesAsJsonString_output.addObjectToReturn(std::string(html));

	std::vector<DeviceResource> resources = repository.getAll("http://example.net/Devices");
	
	mockHttpClient.verify();
	
	EXPECT_EQ(0, resources.size());
}
