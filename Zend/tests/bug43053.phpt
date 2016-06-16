--TEST--
Bug #43053 (Regression: some numbers shown in scientific notation)
--FILE--
<?php

function fn1925939901()
{
    echo 1200000.0 . "\n";
    echo 1300000.0 . "\n";
    echo 1400000.0 . "\n";
    echo 1500000.0 . "\n";
}
fn1925939901();
--EXPECT--
1200000
1300000
1400000
1500000
