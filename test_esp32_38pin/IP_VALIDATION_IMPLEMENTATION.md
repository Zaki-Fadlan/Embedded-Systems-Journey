# IP Validation Implementation - Option 3

## Overview

Implementasi IP validation requirement untuk mencegah IP conflict saat konfigurasi device. User **HARUS validate IP terlebih dahulu** sebelum bisa save config atau factory reset.

---

## Fitur Utama

### 1. **Endpoint `/api/validate-ip` (config_api.h)**

```cpp
GET /api/validate-ip?ip=10.225.180.150
```

**Validasi yang dilakukan:**

**Format IP** - Check IP format valid (xxx.xxx.xxx.xxx)
**Range validasi** - Reject 0.x, 255.x (network/broadcast)
**Subnet check** - IP harus dalam subnet yang sama dengan device
**Gateway check** - IP tidak boleh sama dengan gateway
**Broadcast check** - IP tidak boleh x.x.x.255

**Response Success:**

```json
{
  "available": true,
  "message": " IP validation passed - ready to save",
  "valid": true
}
```

**Response Gagal:**

```json
{
  "error": "IP must be in same subnet as current network",
  "subnet_hint": "10.225.180.x"
}
```

---

### 2. **State Tracking (config_api.h)**

Ditambahkan dua static variables:

```cpp
static String lastValidatedIP = "";        // Track IP yang di-validate
static bool ipValidationPassed = false;    // Apakah validation passed?
```

**Behavior:**

- Validation state di-reset setelah save/reset berhasil
- Validation state di-reset jika user ubah IP input

---

### 3. **Requirement pada Save/Reset (config_api.h)**

#### POST /api/config - REQUIRES validation

```cpp
if (newIP.length() > 0 && !ipValidationPassed) {
    request->send(403, "application/json",
        "{\"error\":\"IP not validated. Please validate IP first.\"}");
    return;
}

if (newIP != lastValidatedIP) {
    request->send(403, "application/json",
        "{\"error\":\"IP changed after validation. Please re-validate.\"}");
    return;
}
```

**Error Codes:**

- `403` - Validation required / IP mismatch
- `400` - Invalid IP format
- `200` - Success

#### POST /api/reset - REQUIRES validation

```cpp
if (!ipValidationPassed) {
    request->send(403, "application/json",
        "{\"error\":\"Cannot factory reset without IP validation...\"}");
    return;
}
```

---

### 4. **Admin Panel UI (admin.html)**

#### Validation Section

```html
<!-- IP Validation Panel -->
<div class="validation-section">
  <strong> IP Address Validation</strong>
  <div class="validation-status">
    <span class="status-icon" id="validationIcon"></span>
    <span id="validationText">Validation pending...</span>
  </div>
</div>
```

#### Button States

```
┌─────────────────────────────────┐
│ [ Validate IP] [ DISABLED] [ DISABLED] │
└─────────────────────────────────┘
      (user clicks validate)
       ↓
┌─────────────────────────────────┐
│ [ Validate IP] [ ENABLED] [ ENABLED]   │
└─────────────────────────────────┘
```

**Button styling:**

- ` Validate IP` - Always enabled (yellow)
- ` Simpan & Restart` - Disabled by default, enabled after validation
- ` Factory Reset` - Disabled by default, enabled after validation

#### Validation Status Display

```
 Validation pending - Please validate IP first
 IP validated - ready to save
 IP conflict detected! Device already using this IP.
```

---

## Workflow User

### Kasus 1: Normal Save (Happy Path)

```
1. Technician buka admin.html
   └─ Validation:  Pending (buttons disabled)

2. Edit form:
   ├─ WiFi SSID: "NewNetwork"
   ├─ Password: "Pass123"
   └─ IP: 10.225.180.150

3. Click " Validate IP"
   └─ API call: GET /api/validate-ip?ip=10.225.180.150
   └─ Response:  Available
   └─ Status: " IP validated - ready to save"
   └─ Buttons: [ ENABLED] [ ENABLED]

4. Click " Simpan & Restart"
   └─ API call: POST /api/config (with WiFi + IP)
   └─ Response:  Success
   └─ Device restart dalam 5 detik
```

### Kasus 2: IP Conflict Detected

```
1. Validate IP
   └─ API call: GET /api/validate-ip?ip=10.225.180.100
   └─ Response:  Already in use by gateway!

2. Error message displayed:
   └─ " Invalid IP range - is gateway"
   └─ Buttons stay DISABLED

3. User harus ubah IP dan validate ulang
```

### Kasus 3: IP Changed After Validation

```
1. Validate IP: 10.225.180.150
   └─ Buttons: ENABLED

2. User ubah IP ke: 10.225.180.151
   └─ Event: onChange listener trigger
   └─ Function: resetValidation()
   └─ Status: " Validation pending - IP changed"
   └─ Buttons: DISABLED again

3. User harus validate IP baru sebelum save
```

### Kasus 4: Factory Reset Requires Validation

```
1. Click " Factory Reset"
   └─ Check: apakah ipValidationPassed == true?

2. JIKA belum validate:
   └─ Confirmation dialog: "Are you sure? (Validation required first)"
   └─ POST /api/reset diblock dengan error 403

3. JIKA sudah validate:
   └─ Confirmation dialog: " Factory reset akan..."
   └─ Proceed dengan reset
```

---

## Error Handling

### Server-side (config_api.h)

| Error Code | Condition         | Message                                            |
| ---------- | ----------------- | -------------------------------------------------- |
| 400        | Invalid IP format | "Invalid IP format"                                |
| 400        | IP range invalid  | "Invalid IP range (avoid 0.x, 255.x)"              |
| 400        | Not in subnet     | "IP must be in same subnet"                        |
| 400        | Is gateway        | "IP is gateway address - cannot use"               |
| 400        | Is broadcast      | "IP is broadcast address - cannot use"             |
| 403        | No validation     | "IP not validated. Please validate IP first."      |
| 403        | IP mismatch       | "IP changed after validation. Please re-validate." |
| 200        | Valid             | "IP validation passed - ready to save"             |

### Client-side (admin.html)

```javascript
// Validation error handling
if (response.ok && result.available) {
     Enable save/reset buttons
} else if (!response.ok) {
     Display error, disable buttons
}

// Network error handling
catch (err) {
     Show "Validation error: " + err
     Keep buttons disabled
}
```

---

## Serial Output Example

```
[IP-VALIDATE]  IP 10.225.180.150 passed validation
[API] POST /api/config received
[API] Restart scheduled in 5 seconds...
[API] Scheduled restart executing...
```

---

## Technical Flow Diagram

```
User Input (IP changed)
    ↓
JavaScript onChange event listener
    ↓
resetValidation() called
    ├─ ipValidationPassed = false
    ├─ lastValidatedIP = ""
    └─ Buttons: DISABLED

User Clicks "Validate IP"
    ↓
validateIP() function
    ├─ Format check
    ├─ API call: GET /api/validate-ip?ip=10.225.180.150
    ├─ Server validation:
    │   ├─ IP format check
    │   ├─ Range validation (0.x, 255.x)
    │   ├─ Subnet check
    │   ├─ Gateway check
    │   └─ Broadcast check
    └─ Response:
        ├─ If : lastValidatedIP = "10.225.180.150", ipValidationPassed = true, Buttons ENABLED
        └─ If : ipValidationPassed = false, Buttons DISABLED

User Clicks "Save & Restart"
    ↓
configForm submit handler
    ├─ POST /api/config with WiFi config
    ├─ Server checks:
    │   ├─ Is ipValidationPassed == true?
    │   └─ Is newIP == lastValidatedIP?
    ├─ If both : Save to NVS, schedule restart
    └─ If any : Return 403 error

Device Restart
    ↓
Boot with new IP
    ├─ Connect to WiFi
    └─ Validation state auto-reset after save successful
```

---

## Keamanan & Best Practices

**User cannot bypass validation** - Save/reset endpoints return 403 if not validated
**IP change detection** - Validation resets if user modifies IP after validation complete
**Prevent double-submit** - Buttons disabled during API calls
**Clear feedback** - User tahu exactly apa gagal dan why
**Graceful degradation** - Network error tetap disable buttons (safe default)

---

## Testing Checklist

- [ ] Validate IP dengan format valid → Success
- [ ] Validate IP dengan format invalid → Error
- [ ] Validate IP out of subnet → Error (subnet hint shown)
- [ ] Validate IP sama dengan gateway → Error
- [ ] Validate IP sama dengan broadcast → Error
- [ ] Change IP setelah validate → Buttons reset disabled
- [ ] Click save tanpa validate → Error 403
- [ ] Click reset tanpa validate → Error 403
- [ ] Validate then save → Success + restart
- [ ] Validate then reset → Success + restart

---

## Implementation Summary

| Component         | Changes                                                                                                                                                                                                                                             |
| ----------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **config_api.h**  | + `/api/validate-ip` endpoint<br>+ Validation logic (subnet, gateway, broadcast checks)<br>+ State tracking (ipValidationPassed, lastValidatedIP)<br>+ Requirement checks in POST /api/config & POST /api/reset                                     |
| **admin.html**    | + Validation section with status display<br>+ Validate IP button (always enabled)<br>+ Save/Reset buttons (disabled by default)<br>+ resetValidation() on IP field change<br>+ validateIP() async function<br>+ Error handling for network failures |
| **main.cpp**      | ✓ No changes (uses existing API handlers)                                                                                                                                                                                                           |
| **credentials.h** | ✓ No changes                                                                                                                                                                                                                                        |
| **nvs_manager.h** | ✓ No changes                                                                                                                                                                                                                                        |

---

## Deploy Instructions

1. **Compile & Upload:**

```bash
python compress_data.py
pio run --target uploadfs  # Upload updated admin.html
pio run --target upload    # Upload updated firmware
```

2. **Verify in Serial Monitor:**

```
[API] Config endpoints registered
...
[IP-VALIDATE]  IP 10.225.180.100 passed validation
```

3. **Test in Browser:**

- Open http://10.225.180.100/admin.html
- Try changing IP → Buttons should be disabled
- Click " Validate IP" → Buttons should enable
- Click " Simpan & Restart" → Should restart with new config

---

**Implementation Complete!** User sekarang tidak bisa save/reset tanpa validate IP dulu.
