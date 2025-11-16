import javax.crypto.*;
import javax.crypto.spec.*;
import java.io.*;
import java.nio.file.*;
import java.security.*;

public class EncryptFile {
  private static final int SALT_LEN = 16;
  private static final int IV_LEN = 12;
  private static final int TAG_BITS = 128;
  private static final int ITERATIONS = 100_000;
  private static final int KEY_BITS = 256;

  public static void encryptFile(Path inPath, char[] password) throws Exception {
    byte[] plaintext = Files.readAllBytes(inPath);

    SecureRandom rnd = SecureRandom.getInstanceStrong();

    byte[] salt = new byte[SALT_LEN];
    rnd.nextBytes(salt);
    byte[] iv = new byte[IV_LEN];
    rnd.nextBytes(iv);

    SecretKeyFactory skf = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256");
    PBEKeySpec spec = new PBEKeySpec(password, salt, ITERATIONS, KEY_BITS);
    SecretKey tmp = skf.generateSecret(spec);
    SecretKeySpec key = new SecretKeySpec(tmp.getEncoded(), "AES");
    spec.clearPassword();

    Cipher cipher = Cipher.getInstance("AES/GCM/NoPadding");
    GCMParameterSpec gspec = new GCMParameterSpec(TAG_BITS, iv);
    cipher.init(Cipher.ENCRYPT_MODE, key, gspec);

    byte[] cipherText = cipher.doFinal(plaintext);

    // Build output path: same dir, same basename + ".bin"
    String name = inPath.getFileName().toString();
    Path outPath = inPath.resolveSibling(name + ".bin");

    try (OutputStream os = Files.newOutputStream(outPath, StandardOpenOption.CREATE,
        StandardOpenOption.TRUNCATE_EXISTING)) {
      os.write("AESG".getBytes("US-ASCII")); // 4 bytes magic
      os.write(0x01); // version
      os.write((byte) SALT_LEN);
      os.write(salt);
      os.write((byte) IV_LEN);
      os.write(iv);
      os.write(cipherText); // ciphertext || tag
    }

    System.out.println("Encrypted -> " + outPath.toString());
  }

  public static void main(String[] args) throws Exception {
    if (args.length < 2) {
      System.err.println("Usage: java EncryptFile <input-file> <password>");
      System.exit(2);
    }
    Path input = Paths.get(args[0]);
    char[] password = args[1].toCharArray();
    encryptFile(input, password);
    java.util.Arrays.fill(password, '\0');
  }
}
