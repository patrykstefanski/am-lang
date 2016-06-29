<?php

function is_prime($n) {
    if ($n < 2) {
        return 0;
    }
    if ($n == 2) {
        return 1;
    }
    $i = 2;
    while ($i < $n) {
        if ($n % $i == 0) {
            return 0;
        }
        $i = $i + 1;
    }
    return 1;
}

echo is_prime(314606869) . "\n";

?>
