--TEST--
goto inside foreach
--FILE--
<?php

function fn1434330578()
{
    foreach ([0] as $x) {
        goto a;
        a:
        echo "loop\n";
    }
    echo "done\n";
}
fn1434330578();
--EXPECT--
loop
done
