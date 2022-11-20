
struct Entry {
  1: string op,
  2: string keyName,
  3: binary encryptedValue
  
}

service KV_RPC {
  oneway void create(1:Entry entry),
  binary access(1:Entry entry),
}
