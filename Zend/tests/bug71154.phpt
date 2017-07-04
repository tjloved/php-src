--TEST--
Bug #71154: Incorrect HT iterator invalidation causes iterator reuse
--FILE--
<?php

function fn2116442993()
{
    $array = [1, 2, 3];
    foreach ($array as &$ref) {
        /* Free array, causing free of iterator */
        $array = [];
        /* Reuse the iterator.
         * However it will also be reused on next foreach iteration */
        $it = new ArrayIterator([1, 2, 3]);
        $it->rewind();
    }
    var_dump($it->current());
}
fn2116442993();
--EXPECT--
int(1)
