--TEST--
goto inside foreach
--FILE--
<?php

function fn1703294428()
{
    foreach ([0] as $x) {
        goto a;
        a:
        echo "loop\n";
    }
    echo "done\n";
}
fn1703294428();
--EXPECT--
loop
done
