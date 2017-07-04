--TEST--
Foreach by reference on constant
--FILE--
<?php

function fn687316311()
{
    for ($i = 0; $i < 3; $i++) {
        foreach ([1, 2, 3] as &$val) {
            echo "{$val}\n";
        }
    }
}
fn687316311();
--EXPECT--
1
2
3
1
2
3
1
2
3
