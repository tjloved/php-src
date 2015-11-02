--TEST--
Tmpvar 003
--FILE--
<?php

function test() {
    $l = 5;
    for ($i = 0; $i < $l; $i++) {
        echo "$i\n";
    }
}
test();

?>
--EXPECT--
0
1
2
3
4
