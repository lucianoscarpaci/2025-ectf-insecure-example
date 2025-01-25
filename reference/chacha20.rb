require 'rbnacl'

message = 'Crypto Example!'
# Generate a random key and nonce
key = RbNaCl::Random.random_bytes(RbNaCl::SecretBox.key_bytes)
nonce = RbNaCl::Random.random_bytes(RbNaCl::SecretBox.nonce_bytes)

# Create a new ChaCha20-Poly1305 instance
secret_box = RbNaCl::SecretBox.new(key)

# Encrypt the message
ciphertext = secret_box.encrypt(nonce, message)

# Decrypt the message
decrypted_message = secret_box.decrypt(nonce, ciphertext)

puts "Original Message: #{message}"
puts "Encrypted Message: #{ciphertext.unpack1('H*')}"
puts "Decrypted Message: #{decrypted_message}"