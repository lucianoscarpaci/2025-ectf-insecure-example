"""
Author: Luciano Scarpaci
Date: 2025
"""

import argparse
import struct
import json
import base64
from wolfcrypt.ciphers import Ed25519Public
from loguru import logger
from wolfcrypt.ciphers import Aes, MODE_CBC


class Encoder:
    def __init__(self, secrets: bytes):

        # Load the json of the secrets file
        secrets = json.loads(secrets)

        # Load the example secrets for use in Encoder.encode
        # This will be "EXAMPLE" in the reference design"
        self.sk = secrets["sk"]
        self.iv = secrets["iv"]
        self.public_key = secrets["public_key"]
        self.signature = secrets["signature"]

        self.validate_secrets()

    def validate_secrets(self):
        """Validate the secrets file

        This function will be called after the secrets file is loaded. You should
        raise an exception if the secrets are invalid
        """
        try:
            # Decode the key, public key and signature from Base64
            sk = base64.b64decode(self.sk)
            iv = base64.b64decode(self.iv)
            sk_iv = sk + iv
            public_key = base64.b64decode(self.public_key)
            signature = base64.b64decode(self.signature)
            public_key = Ed25519Public(public_key)
            public_key.verify(signature, sk_iv)
            logger.success(f"PASSED: The ED25519 signature is valid.")
        except Exception as e:
            logger.error(f"An error occurred during verification: {e}")
            raise

    def encode(self, channel: int, frame: bytes, timestamp: int) -> bytes:

        sk = base64.b64decode(self.sk)
        iv = base64.b64decode(self.iv)
        cipher = Aes(sk, MODE_CBC, iv)
        original_bytes = struct.pack("<IQ", channel, timestamp)
        encrypted_frame = cipher.encrypt(
            frame + (b"\x00" * ((16 - (len(frame) % 16)) % 16))
        )
        encoded_frame = original_bytes + encrypted_frame
        return encoded_frame


def main():

    parser = argparse.ArgumentParser(prog="ectf25_design.encoder")
    parser.add_argument(
        "secrets_file", type=argparse.FileType("rb"), help="Path to the secrets file"
    )
    parser.add_argument("channel", type=int, help="Channel to encode for")
    parser.add_argument("frame", help="Contents of the frame")
    parser.add_argument("timestamp", type=int, help="64b timestamp to use")
    args = parser.parse_args()

    encoder = Encoder(args.secrets_file.read())
    print(repr(encoder.encode(args.channel, args.frame.encode(), args.timestamp)))


if __name__ == "__main__":
    main()
