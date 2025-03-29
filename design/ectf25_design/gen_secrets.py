"""
Author: Luciano Scarpaci
Date: 2025
"""

import argparse
import json
from wolfcrypt.random import Random
from wolfcrypt.ciphers import Ed25519Private
import base64
from pathlib import Path
from loguru import logger


def gen_secrets(channels: list[int]) -> bytes:

    try:
        # Generate a 32-byte secret key (sk)
        sk = Random().bytes(32)
        # Generate a 16-byte initialization vector (iv)
        iv = Random().bytes(16)
        # Concatenate the sk and iv
        sk_iv = sk + iv
        # Generate a new ED25519 signing key
        signing_key = Ed25519Private().make_key(size=32, rng=Random())
        # Derive the public key from the signing key
        public_key = signing_key.encode_key()[1]
        # Extract signature (ED25519) from signed message
        signature = signing_key.sign(sk_iv)

        result = {
            "channels": channels,
            "sk": base64.b64encode(sk).decode("utf-8"),
            "iv": base64.b64encode(iv).decode("utf-8"),
            "public_key": base64.b64encode(public_key).decode("utf-8"),
            "signature": base64.b64encode(signature).decode("utf-8"),
        }
    except Exception as e:
        logger.error(f"An error occured while generating secrets: {e}")
        raise
    return json.dumps(result).encode(), sk, iv, public_key, signature


def parse_args():
    """Define and parse the command line arguments

    NOTE: Your design must not change this function
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--force",
        "-f",
        action="store_true",
        help="Force creation of secrets file, overwriting existing file",
    )
    parser.add_argument(
        "secrets_file",
        type=Path,
        help="Path to the secrets file to be created",
    )
    parser.add_argument(
        "channels",
        nargs="+",
        type=int,
        help="Supported channels. Channel 0 (broadcast) is always valid and will not"
        " be provided in this list",
    )
    return parser.parse_args()


def main():
    """Main function of gen_secrets

    You will likely not have to change this function
    """
    # Parse the command line arguments
    args = parse_args()

    secrets, sk, iv, public_key, signature = gen_secrets(args.channels)

    # Open the file, erroring if the file exists unless the --force arg is provided
    with open(args.secrets_file, "wb" if args.force else "xb") as f:
        # Dump the secrets to the file
        f.write(secrets)

        key_header = args.secrets_file.with_name("secrets.h")
        with open(key_header, "w") as f:
            f.write(f"#ifndef SECRETS_H\n#define SECRETS_H\n\n")
            f.write("#define SK {")
            for i in range(0, len(sk) - 1):
                f.write(hex(sk[i]) + ", ")

            f.write(hex(sk[len(sk) - 1]) + "}\n")
            f.write("#define IV {")
            for i in range(0, len(iv) - 1):
                f.write(hex(iv[i]) + ", ")
            f.write(hex(iv[len(iv) - 1]) + "}\n")
            f.write("#endif\n")


if __name__ == "__main__":
    main()
