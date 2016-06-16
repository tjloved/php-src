--TEST--
Stress test $x .= $x
--FILE--
<?php

function fn448143359()
{
    /*
     * Test case for a concat_function() change that broke a test outside of Zend
     *
     * @see https://github.com/php/php-src/commit/29397f8fd2b4bc8d95e18448ca2d27a62241a407
    **/
    $result = 'f';
    for ($i = 0; $i < 25; ++$i) {
        $result .= $result;
    }
    var_dump(strlen($result));
    echo "Done\n";
}
fn448143359();
--EXPECT--	
int(33554432)
Done

