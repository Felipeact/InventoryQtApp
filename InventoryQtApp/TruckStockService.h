#pragma once
#pragma once

#include "ApiClient.h"

#include <string>
#include <vector>

struct TruckDto
{
    std::string id;
    std::string truckName;
    std::string licensePlate;
    std::string technicianId;
    std::string technicianName;
    std::string status;
};

struct CreateTruckRequest
{
    std::string truckNumber;
    std::string plateNumber;
    std::string technicianId;
    std::string status;
};

struct UpdateTruckRequest
{
    std::string truckNumber;
    std::string plateNumber;
    std::string technicianId;
    std::string status;
};

struct TemplateItemDto
{
    std::string id;
    std::string productName;
    std::string category;
    int requiredQuantity;
    int minimumQuantity;
    double expectedPrice;
    std::string unit;
    std::string notes;
};

struct StockTemplateDto
{
    std::string id;
    std::string name;
    std::string tradeType;
    int itemCount;
};

struct CreateTemplateItemRequest
{
    std::string productName;
    std::string category;
    int requiredQuantity;
    int minimumQuantity;
    double expectedPrice;
    std::string unit;
    std::string notes;
};

struct CreateTemplateRequest
{
    std::string name;
    std::string tradeType;
    std::vector<CreateTemplateItemRequest> items;
};

class TruckStockService
{
public:
    explicit TruckStockService(ApiClient& apiClient);

    std::vector<TruckDto> getTrucks();

    bool createTruck(const CreateTruckRequest& request);
    bool updateTruck(const std::string& truckId,const UpdateTruckRequest& request);
    bool deactivateTruck(const std::string& truckId);

    std::vector<StockTemplateDto> getTemplates();

    bool createTemplate(const CreateTemplateRequest& request);

private:
    ApiClient& apiClient;


};