"""
Author: Ben Janis
Date: 2025

This source file is part of an example system for MITRE's 2025 Embedded System CTF
(eCTF). This code is being provided only for educational purposes for the 2025 MITRE
eCTF competition, and may not meet MITRE standards for quality. Use this code at your
own risk!

Copyright: Copyright (c) 2025 The MITRE Corporation
"""

import argparse
import json
import secrets
import hashlib
import base64
from nacl.signing import SigningKey
from pathlib import Path
from loguru import logger


def gen_secrets(channels: list[int]) -> bytes:
    """Generate the contents secrets file

    This will be passed to the Encoder, ectf25_design.gen_subscription, and the build
    process of the decoder

    :param channels: List of channel numbers that will be valid in this deployment.
        Channel 0 is the emergency broadcast, which will always be valid and will
        NOT be included in this list

    :returns: Contents of the secrets file
    """
    try:
        # Generate an 16-byte sk
        sk = secrets.token_bytes(16)
        # Generate a 16-byte iv
        iv = secrets.token_bytes(16)
        # Generate a new ED25519 signing key
        signing_key = SigningKey.generate()
        # Signs the nonce using the signing key
        signed_message = signing_key.sign(sk + iv)
        # define the public key
        public_key = signing_key.verify_key
        # Extract signature (ED25519) from signed message
        signature = signed_message.signature
        # result including channels, nonce, public key, and signature
        result = {
            "channels": channels,
            "sk": base64.b64encode(sk).decode("utf-8"),
            "iv": base64.b64encode(iv).decode("utf-8"),
            "public_key": base64.b64encode(public_key.encode()).decode("utf-8"),
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

    # Print the generated secrets for your own debugging
    # Attackers will NOT have access to the output of this, but feel free to remove
    #
    # NOTE: Printing sensitive data is generally not good security practice
    logger.debug(f"Generated secrets: {secrets}")

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

    # For your own debugging. Feel free to remove
    logger.success(f"Wrote secrets to {str(args.secrets_file.absolute())}")


if __name__ == "__main__":
    main()
