--TEST--
ZE2 iterators cannot implement Traversable alone
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class test implements Traversable
{
}
function fn2134249765()
{
    $obj = new test();
    foreach ($obj as $v) {
    }
    print "Done\n";
    /* the error doesn't show the filename but 'Unknown' */

}
fn2134249765();
--EXPECTF--
Fatal error: Class test must implement interface Traversable as part of either Iterator or IteratorAggregate in %s on line %d
