--TEST--
Closure 010: Closure calls itself
--FILE--
<?php

function fn39950554()
{
    $i = 3;
    $lambda = function ($lambda) use(&$i) {
        if ($i == 0) {
            return;
        }
        echo $i-- . "\n";
        $lambda($lambda);
    };
    $lambda($lambda);
    echo "{$i}\n";
}
fn39950554();
--EXPECT--
3
2
1
0
