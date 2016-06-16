--TEST--
Testing do-while loop
--FILE--
<?php

function fn195024939()
{
    $i = 3;
    do {
        echo $i;
        $i--;
    } while ($i > 0);
}
fn195024939();
--EXPECT--
321
