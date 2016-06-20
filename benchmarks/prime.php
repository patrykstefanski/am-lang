<?php

function is_prime($n) {
    $i = 2;
    $m = $n / 2;
    while ($i < $m) {
        if ($n % $i == 0) {
            return 0;
        }
        $i = $i + 1;
    }
    return 1;
}

echo is_prime(314606869) . "\n";

?>
