#ifndef SAC_DATA_H
#define SAC_DATA_H

class sac_data_t {
  public:
    enum encoding {
      DD4A = 1,
      DD8A = 2
    };

    sac_data_t(
        uint8_t *data,
        int size,
        int num_samples,
        int num_channels,
        int sample_rate,
        encoding data_encoding)
        : m_data(data),
          m_size(size),
          m_num_samples(num_samples),
          m_num_channels(num_channels),
          m_sample_rate(sample_rate),
          m_data_encoding(data_encoding) {}

    ~sac_data_t() {
      if (m_data)
        delete[] m_data;
    }

    uint8_t *data() const {
      return m_data;
    }

    int size() const {
      return m_size;
    }

    int num_samples() const {
      return m_num_samples;
    }

    int num_channels() const {
      return m_num_channels;
    }

    int sample_rate() const {
      return m_sample_rate;
    }

    encoding data_encoding() const {
      return m_data_encoding;
    }

  private:
    sac_data_t();
    sac_data_t(const sac_data_t& other);
    sac_data_t& operator=(const sac_data_t& other);

    uint8_t *m_data;
    int m_size;

    int m_num_samples;
    int m_num_channels;
    int m_sample_rate;

    encoding m_data_encoding;
};

#endif // SAC_DATA_H
