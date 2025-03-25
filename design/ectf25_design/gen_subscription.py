"""
Author: Luciano Scarpaci
Date: 2025
"""

import argparse
import json
from pathlib import Path
import struct
from wolfcrypt.ciphers import Ed25519Public
import base64
from loguru import logger


def gen_subscription(
    secrets: bytes, device_id: int, start: int, end: int, channel: int
) -> bytes:

    secrets = json.loads(secrets)
    sk = secrets["sk"]
    iv = secrets["iv"]
    public_key = secrets["public_key"]
    signature = secrets["signature"]
    # ED25519 signature verification
    try:
        # Decode the key, public key and signature from Base64
        sk = base64.b64decode(sk)
        iv = base64.b64decode(iv)
        sk_iv = sk + iv
        public_key = base64.b64decode(public_key)
        signature = base64.b64decode(signature)
        public_key = Ed25519Public(public_key)
        public_key.verify(signature, sk_iv)
        logger.success(f"PASSED: The ED25519 signature is valid.")
    except Exception as e:
        logger.error(f"An error occured while verifying the ED25519 signature: {e}")
        raise

    subscription_bytes = struct.pack("<IQQI", device_id, start, end, channel)
    return subscription_bytes


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--force",
        "-f",
        action="store_true",
        help="Force creation of subscription file, overwriting existing file",
    )
    parser.add_argument(
        "secrets_file",
        type=argparse.FileType("rb"),
        help="Path to the secrets file created by ectf25_design.gen_secrets",
    )
    parser.add_argument("subscription_file", type=Path, help="Subscription output")
    parser.add_argument(
        "device_id", type=lambda x: int(x, 0), help="Device ID of the update recipient."
    )
    parser.add_argument(
        "start", type=lambda x: int(x, 0), help="Subscription start timestamp"
    )
    parser.add_argument("end", type=int, help="Subscription end timestamp")
    parser.add_argument("channel", type=int, help="Channel to subscribe to")
    return parser.parse_args()


def main():
    # Parse the command line arguments
    args = parse_args()

    subscription = gen_subscription(
        args.secrets_file.read(), args.device_id, args.start, args.end, args.channel
    )
    # Open the file, erroring if the file exists unless the --force arg is provided
    with open(args.subscription_file, "wb" if args.force else "xb") as f:
        f.write(subscription)


if __name__ == "__main__":
    main()
