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

using ::testing::Return;

class MockHttpClient : public IHttpRestClient
{
public:
	MOCK_CONST_METHOD1(getResourcesAsJsonString, std::string(const std::string &uri));
};

TEST(DeviceResourceNetworkRepository, Positive)
{
	char *html = "[{\"domainName\" : \"1E\", \"netbiosName\" : \"testMachine\"}]";
	MockHttpClient mockHttpClient;
	DeviceResourceNetworkRepository repository(mockHttpClient);
	EXPECT_CALL(mockHttpClient, getResourcesAsJsonString("http://example.net/Devices"))
		.WillOnce(Return(std::string(html)));
	
	std::vector<DeviceResource> resources = repository.getAll("http://example.net/Devices");
	EXPECT_EQ(1, resources.size());
	EXPECT_STREQ("1E", resources[0].domainName.c_str());
}

TEST(DeviceResourceNetworkRepository, Negative) 
{
	char *html = "";

	MockHttpClient mockHttpClient;
	DeviceResourceNetworkRepository repository(mockHttpClient);

	EXPECT_CALL(mockHttpClient, getResourcesAsJsonString("http://example.net/Devices"))
		.WillOnce(Return(std::string(html)));

	std::vector<DeviceResource> resources = repository.getAll("http://example.net/Devices");
	EXPECT_EQ(0, resources.size());
}
