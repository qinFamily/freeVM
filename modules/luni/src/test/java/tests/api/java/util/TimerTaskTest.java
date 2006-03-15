/* Copyright 1998, 2005 The Apache Software Foundation or its licensors, as applicable
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package tests.api.java.util;

import java.util.Timer;
import java.util.TimerTask;

public class TimerTaskTest extends junit.framework.TestCase {
	Object sync = new Object(), start = new Object();

	/**
	 * Warning: These tests have the possibility to leave a VM hanging if the
	 * Timer is not cancelled.
	 */
	class TimerTestTask extends TimerTask {
		private int wasRun = 0;

		// Set this to true to see normal tests fail (or hang possibly)
		// The default is false and needs to be set by some tests
		private boolean sleepInRun = false;

		private boolean cancelCalled = false;

		public void run() {
			synchronized (this) {
				wasRun++;
			}
			synchronized (start) {
				start.notify();
			}
			if (sleepInRun) {
				
				try {
					Thread.sleep(200);
				} catch (InterruptedException e) {
				}
			}
			synchronized (sync) {
				sync.notify();
			}
		}

		public synchronized int wasRun() {
			return wasRun;
		}
		
		public boolean cancel() {
			this.cancelCalled = true;
			return super.cancel();
		}

		public synchronized boolean cancelled() {
			return cancelCalled;
		}

		public void sleepInRun(boolean value) {
			sleepInRun = value;
		}
	}

	/**
	 * @tests java.util.TimerTask#TimerTask()
	 */
	public void test_Constructor() {
		// Ensure the constructor does not fail
		new TimerTestTask();
	}

	/**
	 * @tests java.util.TimerTask#cancel()
	 */
	public void test_cancel() {
		Timer t = null;
		try {
			// Ensure cancel returns false if never scheduled
			TimerTestTask testTask = new TimerTestTask();
			assertTrue("Unsheduled tasks should return false for cancel()",
					!testTask.cancel());

			// Ensure cancelled task never runs
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 500);
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
			}
			assertTrue("TimerTask should not have run yet", testTask.cancel());
			try {
				Thread.sleep(500);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.run() method should not be called after cancel()",
					testTask.wasRun() == 0);
			t.cancel();

			// Ensure cancelling a task which has already run returns true
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 50);
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.cancel() should return false if task has run",
					!testTask.cancel());
			t.cancel();

			// Ensure cancelling a task which is has already run returns true
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 50);
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.cancel() should return false if task has run",
					!testTask.cancel());
			assertTrue(
					"TimerTask.cancel() should return false if called a second time",
					!testTask.cancel());
			t.cancel();

			// Ensure cancelling a repeated execution task which has never run
			// returns true
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 500, 500); // should never run
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.cancel() should return true if sheduled for repeated execution even if not run",
					testTask.cancel());
			t.cancel();

			// Ensure cancelling a repeated execution task which HAS run returns
			// true
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 50, 50); // should run at least two times
			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.cancel() should return true if sheduled for repeated execution and run",
					testTask.cancel());
			assertTrue("TimerTask should have run at least twice", testTask
					.wasRun() > 2);
			int wasRunCount = testTask.wasRun();
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
			}
			assertTrue("TimerTask cancelled should not run again", testTask
					.wasRun() == wasRunCount);
			t.cancel();

			// Ensure calling cancel a second returns false
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 5000); // Should never run
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.cancel() should return true if task has never run",
					testTask.cancel());
			assertTrue(
					"TimerTask.cancel() should return false if called a second time",
					!testTask.cancel());
			t.cancel();

			// Ensure cancelling a task won't cause deadlock
			t = new Timer();
			testTask = new TimerTestTask();
			testTask.sleepInRun(true);
			synchronized (start) {
				t.schedule(testTask, 0);
				try {
					start.wait();
					Thread.sleep(50);
				} catch (InterruptedException e) {
				}
			}
			testTask.cancel();
			assertTrue("TimerTask should have been cancelled", testTask
					.cancelled());
			t.cancel();
		} finally {
			if (t != null)
				t.cancel();
		}
	}

	/**
	 * @tests java.util.TimerTask#scheduledExecutionTime()
	 */
	public void test_scheduledExecutionTime() {
		Timer t = null;
		try {
			// Ensure scheduledExecutionTime is roughly right
			t = new Timer();
			TimerTestTask testTask = new TimerTestTask();
			t.schedule(testTask, 100);
			long time = System.currentTimeMillis() + 100;
			synchronized (sync) {
				try {
					sync.wait(500);
				} catch (InterruptedException e) {
				}
			}
			long scheduledExecutionTime = testTask.scheduledExecutionTime();
			assertTrue("scheduledExecutionTime not too accurate "
					+ scheduledExecutionTime + " vs time " + time,
					(scheduledExecutionTime > time - 50)
							&& scheduledExecutionTime <= time);
			t.cancel();

			// Ensure scheduledExecutionTime is the last scheduled time
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 100, 500);
			long estNow = System.currentTimeMillis() + 100;
			// Will wake in 100, and every 500 run again
			// We want to try to get it after it's run at least once but not
			// twice
			synchronized (sync) {
				try {
					sync.wait(500);
				} catch (InterruptedException e) {
				}
			}
			scheduledExecutionTime = testTask.scheduledExecutionTime();
			assertTrue("scheduledExecutionTime should be last time it was run",
					(scheduledExecutionTime > estNow - 50)
							&& scheduledExecutionTime <= estNow);
			t.cancel();
		} finally {
			if (t != null)
				t.cancel();
		}

	}

	/**
	 * @tests java.util.TimerTask#run()
	 */
	public void test_run() {
		Timer t = null;
		try {
			// Ensure a new task is never run
			TimerTestTask testTask = new TimerTestTask();
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
			}
			assertTrue("TimerTask.run() method should not have been called",
					testTask.wasRun() == 0);

			// Ensure a task is run
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 200);
			try {
				Thread.sleep(400);
			} catch (InterruptedException e) {
			}
			assertTrue("TimerTask.run() method not called after 200ms",
					testTask.wasRun() == 1);
			t.cancel();

			// Ensure a repeated execution task does just that
			t = new Timer();
			testTask = new TimerTestTask();
			t.schedule(testTask, 50, 50);
			try {
				Thread.sleep(400);
			} catch (InterruptedException e) {
			}
			assertTrue(
					"TimerTask.run() method should have been called at least 4 times",
					testTask.wasRun() >= 4);
			t.cancel();
		} finally {
			if (t != null)
				t.cancel();
		}

	}

	protected void setUp() {
	}

	protected void tearDown() {
	}
}
