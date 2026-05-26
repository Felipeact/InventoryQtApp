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

struct AssignmentTruckDto
{
    std::string id;
    std::string truckNumber;
};

struct AssignmentTemplateDto
{
    std::string id;
    std::string name;
};

struct CreateAssignmentRequest
{
    std::string truckId;
    std::string templateId;
};

struct TruckAssignmentDto
{
    std::string id;
    std::string truckNumber;
    std::string templateName;
    std::string assignedOn;
    std::string assignedBy;
    std::string status;
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

    bool assignTemplate(const CreateAssignmentRequest& request);
    std::vector<TruckAssignmentDto> getAssignments();

private:
    ApiClient& apiClient;


};