
class hist
{
public:
	hist(const int w = 128, const int h = 1024)
		: m_height(h)
		, m_width(w)
	{
		m_data = new quint64[w * h];
		m_column = new quint64[w];
		for (int i = 0; i < w; ++i)
			m_column[i] = i * w;
		clear();
	}

	void clear(void)
	{
		memset(m_data, '\0', m_width * m_height * sizeof(quint64));
	}

	bool incVal(int x, int y)
	{
		// quint32 pos = y * m_width + x;
		// ++m_data[pos];
		quint64 pos = y + m_column[x];
		++m_data[pos];
		return true;
	}

	quint64 getTotalCounts(void)
	{
		quint64 sum(0);
		for (quint32 i = 0; i < m_height * m_width; ++i)
			sum += m_data[i];
		return sum;
	}

private:
	quint64	*m_data;

	quint64	*m_column;

	quint32	m_height;

	quint32	m_width;
};
