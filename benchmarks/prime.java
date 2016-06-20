public class prime {
    private static long isPrime(long n) {
        long i = 2;
        long m = n / 2;
        while (i < m) {
            if (n % i == 0) {
                return 0;
            }
            i = i + 1;
        }
        return 1;
    }

    public static void main(String[] args) {
        System.out.println(isPrime(314606869));
    }
}
