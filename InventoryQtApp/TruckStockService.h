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

struct TemplateDetailsDto
{
    std::string id;
    std::string name;
    std::string tradeType;
    std::vector<CreateTemplateItemRequest> items;
};

struct MyTruckStockItemDto
{
    std::string id;
    std::string productName;
    std::string category;
    int currentQuantity;
    int minimumQuantity;
    int requiredQuantity;
    std::string status;
};

struct MyTruckStockDto
{
    std::string truckId;
    std::string truckNumber;
    std::string plateNumber;
    std::vector<MyTruckStockItemDto> items;
};


struct UseTruckItemRequest
{
    std::string itemId;
    int quantityUsed;
    std::string notes;
};

struct LowStockItemDto
{
    std::string templateName;
    std::string truckNumber;
    std::string productName;
    int currentQuantity;
    int minimumQuantity;
    std::string status;
};

struct ReceiptDto
{
    std::string id;
    std::string technicianName;
    std::string truckNumber;
    double totalAmount;
    std::string status;
    std::string createdAt;
    std::string fileUrl;
};

struct CreateReceiptRequest
{
    std::string truckId;
    std::string fileUrl;
    double totalAmount;
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

    TemplateDetailsDto getTemplateById(const std::string& templateId);

    bool updateTemplate(
        const std::string& templateId,
        const CreateTemplateRequest& request
    );

    bool deleteTemplate(const std::string& templateId);

    MyTruckStockDto getMyTruckStock();

    bool useTruckItem(const UseTruckItemRequest& request);

    std::vector<LowStockItemDto> getLowStockItems();

    std::vector<ReceiptDto> getReceipts();

    bool createReceipt(
        const CreateReceiptRequest& request
    );

    bool approveReceipt(const std::string& receiptId);

private:
    ApiClient& apiClient;


};