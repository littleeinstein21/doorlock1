#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

class Fingerprint {
public:
    Fingerprint(uint8_t rx, uint8_t tx)
        : mySerial(rx, tx), finger(&mySerial) {}

    void begin(uint32_t baud) {
        mySerial.begin(baud);
        finger.begin(baud);
    }

    bool verifyPassword() {
        return finger.verifyPassword();
    }

    bool enrollFingerprint(uint8_t id) {
        while (finger.getImage() != FINGERPRINT_OK);
        if (finger.image2Tz(1) != FINGERPRINT_OK) return false;

        while (finger.getImage() != FINGERPRINT_NOFINGER);
        while (finger.getImage() != FINGERPRINT_OK);
        if (finger.image2Tz(2) != FINGERPRINT_OK) return false;

        if (finger.createModel() != FINGERPRINT_OK) return false;
        if (finger.storeModel(id) != FINGERPRINT_OK) return false;
        return true;
    }

    bool deleteFingerprint(uint8_t id) {
        return finger.deleteModel(id) == FINGERPRINT_OK;
    }

    bool isFingerprintMatched(uint8_t id) {
        if (finger.getImage() != FINGERPRINT_OK) return false;
        if (finger.image2Tz() != FINGERPRINT_OK) return false;
        if (finger.fingerFastSearch() != FINGERPRINT_OK) return false;
        if (finger.fingerID == id) return true;
        return false;
    }

private:
    SoftwareSerial mySerial;
    Adafruit_Fingerprint finger;
};
