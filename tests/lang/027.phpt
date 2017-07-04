--TEST--
Testing do-while loop
--FILE--
<?php

function fn1929303283()
{
    $i = 3;
    do {
        echo $i;
        $i--;
    } while ($i > 0);
}
fn1929303283();
--EXPECT--
321
