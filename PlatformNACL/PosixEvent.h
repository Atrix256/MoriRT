/*
//from http://qstuff.blogspot.com/2005/05/events-posix-version.html


class CEvent // POSIX version of event: condition variable (fast?)
{
private:
	bool m_bAutoReset; // Constant

	pthread_mutex_t m_mutex; // Mutex
	pthread_cond_t m_cond; // Condition variable

	// Protected by m_mutex
	bool m_bSet; // Set or clear

	unsigned int m_nWaitingThreads; // Number of threads waiting on the event

	// As the name implies, this must be called inside the mutex
	// Does the wait. The parameter specifies when the thread should wake up, should the event not get set before then. If this is NULL, the thread will wait indefinitely on the event. Returns whether the event got set (if not, the timeout must have expired).
	bool InnerWait(const timespec *restrict abstime)
	{
		if (!m_bSet)
		{
			m_nWaitingThreads++;

			do
			{
				int nRetVal;

				// Do the wait, either timed or indefinite
				if (abstime)
					nRetVal = pthread_cond_timedwait(&m_cond, &m_mutex, abstime);
				else
					nRetVal = pthread_cond_wait(&m_cond, &m_mutex);

				assert(nRetVal == 0 || nRetVal == ETIMEDOUT);
			}
			while (!m_bSet && nRetVal != ETIMEDOUT); // Loop until it gets set or the timeout expires

			m_nWaitingThreads--;
		}

		// Did the event get set?
		bool bSuccess = m_bSet;
		// If the event is set and it's an auto-reset event, reset it now that we're awake
		if (m_bSet && m_bAutoReset)
		m_bSet = false;

		return bSuccess;
	}

public:
	CEvent(bool bAutoReset, bool bSet)
	{
		if (pthread_mutex_init(&m_mutex, NULL) != 0)
			throw std::bad_alloc("Unable to create mutex");
		else if (pthread_cond_init(&m_cond, NULL) != 0)
		{
			pthread_mutex_destroy(&m_mutex);

			throw std::bad_alloc("Unable to create condition variable");
		}

		m_bAutoReset = bAutoReset;
		m_bSet = bSet;

		m_nWaitingThreads = 0;
	}

	inline ~CEvent()
	{
		pthread_cond_destroy(&m_cond);
		pthread_mutex_destroy(&m_mutex);
	}

	void Set()
	{
		pthread_mutex_lock(&m_mutex);

		if (!m_bSet) // If it's already set, do nothing
		{
			m_bSet = true; // Set the event

			// Check if there are any waiters, and release them appropriately
			if (m_nWaitingThreads)
			{
				if (m_bAutoReset)
					pthread_cond_signal(&m_cond); // Release one thread
				else
					pthread_cond_broadcast(&m_cond); // Release all threads
			}
		}

		pthread_mutex_unlock(&m_mutex);
	}

	inline void Reset()
	{
		pthread_mutex_lock(&m_mutex);

		m_bSet = false; // Ding

		pthread_mutex_unlock(&m_mutex);
	}

	bool Wait(unsigned int nTimeoutMS)
	{
		// Calculate the time to wake based on the time to sleep. I hope I understand how this is supposed to work on POSIX.
		timespec now, timeout, later;

		clock_gettime(CLOCK_REALTIME, &now);

		timeout.tv_sec = nTimeoutMS / 1000; // Seconds
		timeout.tv_nsec = (nTimeoutMS % 1000) * 1000000L; // Nanoseconds

		later.tv_sec = now.tv_sec + timeout.tv_sec;
		later.tv_nsec = now.tv_nsec + timeout.tv_nsec;
		if (later.tv_nsec >= 1000000000L)
		{
			later.tv_nsec -= 1000000000L;
			later.tv_sec++;
		}

		pthread_mutex_lock(&m_mutex);

		bool bSuccess = InnerWait(&later);

		pthread_mutex_unlock(&m_mutex);

		return bSuccess;
	}

	inline void Wait()
	{
		pthread_mutex_lock(&m_mutex);

		InnerWait(NULL);

		pthread_mutex_unlock(&m_mutex);
	}
};*/